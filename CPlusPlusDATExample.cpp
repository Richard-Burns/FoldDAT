/* Shared Use License: This file is owned by Derivative Inc. (Derivative) and
* can only be used, and/or modified for use, in conjunction with
* Derivative's TouchDesigner software, and only if you are a licensee who has
* accepted Derivative's TouchDesigner license or assignment agreement (which
* also govern the use of this file).  You may share a modified version of this
* file with another authorized licensee of Derivative's TouchDesigner software.
* Otherwise, no redistribution or sharing of this file, with or without
* modification, is permitted.
*/

#include "CPlusPlusDATExample.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <array>
#include <iostream>

// These functions are basic C function, which the DLL loader can find
// much easier than finding a C++ Class.
// The DLLEXPORT prefix is needed so the compile exports these functions from the .dll
// you are creating
extern "C"
{

DLLEXPORT
void
FillDATPluginInfo(DAT_PluginInfo *info)
{
	// Always return DAT_CPLUSPLUS_API_VERSION in this function.
	info->apiVersion = DATCPlusPlusAPIVersion;

	// The opType is the unique name for this TOP. It must start with a
	// capital A-Z character, and all the following characters must lower case
	// or numbers (a-z, 0-9)
	info->customOPInfo.opType->setString("Fold");

	// The opLabel is the text that will show up in the OP Create Dialog
	info->customOPInfo.opLabel->setString("Fold");

	// Will be turned into a 3 letter icon on the nodes
	info->customOPInfo.opIcon->setString("FDT");

	// Information about the author of this OP
	info->customOPInfo.authorName->setString("Richard Burns");
	info->customOPInfo.authorEmail->setString("richard@richard-burns.com");

	// This DAT works with 0 or 1 inputs
	info->customOPInfo.minInputs = 1;
	info->customOPInfo.maxInputs = 2;

}

DLLEXPORT
DAT_CPlusPlusBase*
CreateDATInstance(const OP_NodeInfo* info)
{
	// Return a new instance of your class every time this is called.
	// It will be called once per DAT that is using the .dll
	return new CPlusPlusDATExample(info);
}

DLLEXPORT
void
DestroyDATInstance(DAT_CPlusPlusBase* instance)
{
	// Delete the instance here, this will be called when
	// Touch is shutting down, when the DAT using that instance is deleted, or
	// if the DAT loads a different DLL
	delete (CPlusPlusDATExample*)instance;
}

};

CPlusPlusDATExample::CPlusPlusDATExample(const OP_NodeInfo* info) : myNodeInfo(info)
{
}

CPlusPlusDATExample::~CPlusPlusDATExample()
{
}

void
CPlusPlusDATExample::getGeneralInfo(DAT_GeneralInfo* ginfo, const OP_Inputs* inputs, void* reserved1)
{
	// This will cause the node to cook every frame
	ginfo->cookEveryFrameIfAsked = false;
}

void
CPlusPlusDATExample::execute(DAT_Output* output,
							const OP_Inputs* inputs,
							void* reserved)
{
	
	// If only input 0 is connected we enable the Fold parameter. If both inputs are connected we only enable the Interleave parameter.
	inputs->enablePar("Fold", 1);
	inputs->enablePar("Interleave", 1);
	output->setOutputDataType(DAT_OutDataType::Text);
	
	if (!output)
		return;

	// check what inputs we have and check their type
	bool input0Enabled = inputs->getInputDAT(0);
	bool input1Enabled = inputs->getInputDAT(1);
	bool input0IsTable = inputs->getInputDAT(0)->isTable;
	bool input1IsTable = inputs->getInputDAT(1)->isTable;

	// setup easy to use vars for input
	const OP_DATInput* cinput0 = inputs->getInputDAT(0);
	const OP_DATInput* cinput1 = inputs->getInputDAT(1);


	// only input 0 connected
	
	if (input0Enabled && !input1Enabled)
	{
		inputs->enablePar("Fold", 1);
		inputs->enablePar("Interleave", 0);
		int foldChoice = inputs->getParInt("Fold");

		// if input 0 is a table re-organise depending on Fold menu choice.
		if (input0IsTable) 
		{
			int numRows = cinput0->numRows;
			int numCols = cinput0->numCols;

			output->setOutputDataType(DAT_OutDataType::Table);

			int numCells = cinput0->numRows*cinput0->numCols;
			int r = 0;

			switch (foldChoice)
			{

				// rows to single column
				case 0:
					reOrder(output, cinput0, numCells, false, false, false);
					break;

				// rows to single row
				case 1:
					reOrder(output, cinput0, numCells, true, false, true);
					break;

				// columns to single column
				case 2:
					reOrder(output, cinput0, numCells, false, true, false);
					break;
				
				// columns to single row	
				case 3:
					reOrder(output, cinput0, numCells, true, true, true);
					break;

			}
		}
		// if input 0 is a text DAT use the fold DAT as a standard pass through as other DATs seem to.
		else 
		{

			const char* str = cinput0->getCell(0, 0);
			output->setText(str);

			/*
			output->setTableSize(1, 1);
			output->setCellString(0, 0, cinput0->getCell(0, 0));
			output->setOutputDataType(DAT_OutDataType::Table);
			*/
		}

	}


	 // INTERLEAVE

	
	/*
	if (input0Enabled && input1Enabled)
	{
		inputs->enablePar("Fold", 0);
		inputs->enablePar("Interleave", 1);

		int interChoice = inputs->getParInt("Interleave");
		int i = inputs->getNumInputs();
	
		int numRows = cinput0->numRows;
		int numCols = cinput0->numCols;
		int numRows2 = cinput1->numRows;
		int numCols2 = cinput1->numCols;
		int totalRows = numRows + numRows2;
		int totalCols = numCols + numCols2;
		int maxRows = numRows;
		int maxCols = numCols;

		if (numRows2 > numRows) {
			maxRows = numRows2;
		}

		if (numCols2 > numCols) {
			maxCols = numCols2;
		}


		if (!input1IsTable) // is Text
		{
			const char* str = cinput0->getCell(0, 0);
			output->setText(str);
		}
		else {

			output->setOutputDataType(DAT_OutDataType::Table);

			int r = 0;
			int r2 = 0;
			int rowsAvailable = 0;
			int curRow = 0;

			switch (interChoice) {

			case 0:

				output->setTableSize(totalRows, maxCols);

				for (int i = 0; i < maxRows; i++) {
					if (r < numRows) {
						for (int j = 0; j < cinput0->numCols; j++)
						{
							const char* str = cinput0->getCell(r, j);
							output->setCellString(curRow, j, str);
						}
						r++;
						curRow++;
					}

					if (r2 < numRows2) {
						for (int j = 0; j < cinput1->numCols; j++)
						{
							const char* str = cinput1->getCell(r2, j);
							output->setCellString(curRow, j, str);
						}
						r2++;
						curRow++;
					}

				}

				break;


			case 1:

				output->setTableSize(maxRows, totalCols);

				for (int i = 0; i < maxCols; i++) {
					if (r < numCols) {
						for (int j = 0; j < cinput0->numRows; j++)
						{
							const char* str = cinput0->getCell(j, r);
							output->setCellString(j, curRow, str);
						}
						r++;
						curRow++;
					}

					if (r2 < numCols2) {
						for (int j = 0; j < cinput1->numRows; j++)
						{
							const char* str = cinput1->getCell(j, r2);
							output->setCellString(j, curRow, str);
						}
						r2++;
						curRow++;
					}

				}

				break;
			}
		}

	}
	*/
}

int32_t
CPlusPlusDATExample::getNumInfoCHOPChans(void* reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 0;
}

void
CPlusPlusDATExample::getInfoCHOPChan(int32_t index,
									OP_InfoCHOPChan* chan, void* reserved1)
{
	// This function will be called once for each channel we said we'd want to return
	// In this example it'll only be called once.
}

bool
CPlusPlusDATExample::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
	infoSize->rows = 0;
	infoSize->cols = 0;
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void
CPlusPlusDATExample::getInfoDATEntries(int32_t index,
									int32_t nEntries,
									OP_InfoDATEntries* entries,
									void* reserved1)
{

}

void
CPlusPlusDATExample::setupParameters(OP_ParameterManager* manager, void* reserved1)
{

	// Fold Menu
	{
		OP_StringParameter	np;

		np.name = "Fold";
		np.label = "Fold";
		np.page = "Fold";

		const char* names[] = { "Rowstocol", "Rowstorow", "Colstocol", "Colstorow" };
		const char* labels[] = { "Rows to Single Column", "Rows to Single Row", "Columns to Single Column", "Columns to Single Row" };

		OP_ParAppendResult res = manager->appendMenu(np, 4, names, labels);
		assert(res == OP_ParAppendResult::Success);
	}

	// Interleave Menu
	
	{
		OP_StringParameter	np;

		np.name = "Interleave";
		np.label = "Interleave";
		np.page = "Fold";

		const char* names[] = { "Interleaverows", "Interleavecols"};
		const char* labels[] = { "Interleave Rows", "Interleave Columns" };

		OP_ParAppendResult res = manager->appendMenu(np, 2, names, labels);
		assert(res == OP_ParAppendResult::Success);
	}
}

void
CPlusPlusDATExample::pulsePressed(const char* name, void* reserved1)
{
}

DAT_Output* CPlusPlusDATExample::reOrder(DAT_Output* output, const OP_DATInput* input, int numCells, bool tableFlip, int loopFlip, int orderFlip)
{
	output->setTableSize(numCells, 1);
	if (tableFlip) 
	{
		output->setTableSize(1, numCells);
	}

	int r = 0;
	int nr = input->numRows;
	int nc = input->numCols;

	int f1 = nr;
	int f2 = nc;

	if (loopFlip) 
	{
		f1 = nc;
		f2 = nr;
	}

	for (int i = 0; i < f1; i++)
	{
		for (int j = 0; j < f2; j++)
		{
			const char* str = input->getCell(i, j);
			if (loopFlip) 
			{
				str = input->getCell(j, i);
			}
			
			output->setCellString(r, 0, str);
			
			if (orderFlip)
			{
				output->setCellString(0, r, str);
			}
			r++;
		}
	}
	return output;
}