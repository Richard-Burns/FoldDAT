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
	info->customOPInfo.maxInputs = 1;

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
	if (!output)
		return;

	int input1Enabled = -1;

	if (inputs->getInputDAT(0)) {
		input1Enabled = 1;
	}

	/*
	int input2Enabled = -1;

	if (inputs->getInputDAT(1)->opId) {
		input2Enabled = 1;
	}*/


	if (input1Enabled)
	{
		inputs->enablePar("Fold", 1);
		//inputs->enablePar("Interleave", 0);
		int foldChoice = inputs->getParInt("Fold");

		const OP_DATInput	*cinput = inputs->getInputDAT(0);

		bool isTable = cinput->isTable;
		
		if (isTable) {
			int numRows = cinput->numRows;
			int numCols = cinput->numCols;

			output->setOutputDataType(DAT_OutDataType::Table);

			int numCells = cinput->numRows*cinput->numCols;
			int r = 0;

			switch (foldChoice) {

				case 0:

					output->setTableSize(numCells, 1);

					for (int i = 0; i < cinput->numRows; i++)
					{
						for (int j = 0; j < cinput->numCols; j++)
						{
							const char* str = cinput->getCell(i, j);
							output->setCellString(r, 0, str);
							r++;
						}
					}
					break;
				case 1:

					output->setTableSize(1, numCells);

					for (int i = 0; i < cinput->numRows; i++)
					{
						for (int j = 0; j < cinput->numCols; j++)
						{
							const char* str = cinput->getCell(i, j);
							output->setCellString(0, r, str);
							r++;
						}
					}
					break;

				case 2:

					output->setTableSize(numCells, 1);

					for (int i = 0; i < cinput->numCols; i++)
					{
						for (int j = 0; j < cinput->numRows; j++)
						{
							const char* str = cinput->getCell(j, i);
							output->setCellString(r, 0, str);
							r++;
						}
						//r++;
					}
					break;
					
				case 3:
					output->setTableSize(1, numCells);

					for (int i = 0; i < cinput->numCols; i++)
					{
						for (int j = 0; j < cinput->numRows; j++)
						{
							const char* str = cinput->getCell(j, i);
							output->setCellString(0, r, str);
							r++;
						}
						//r++;
					}
					break;

			}
		}

		else {
			const char* str = cinput->getCell(0, 0);
			output->setText(str);
		}

	}
	 // INTERLEAVE
	/*
	if (input1Enabled && input2Enabled)
	{
		//inputs->enablePar("Rows", 0);		// not used
		//inputs->enablePar("Cols", 0);		// not used
		//inputs->enablePar("Outputtype", 0);	// not used
		inputs->enablePar("Fold", 0);
		inputs->enablePar("Interleave", 1);

		int interChoice = inputs->getParInt("Interleave");
		int i = inputs->getNumInputs();

		const OP_DATInput *cinput = inputs->getInputDAT(0);
		const OP_DATInput *cinput2 = inputs->getInputDAT(1);
	

		if (cinput && cinput2) {
			int numRows = cinput->numRows;
			int numCols = cinput->numCols;
			int numRows2 = cinput2->numRows;
			int numCols2 = cinput2->numCols;
			bool isTable = cinput->isTable;
			bool isTable2 = cinput2->isTable;
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


			if (!isTable2) // is Text
			{
				const char* str = cinput->getCell(0, 0);
				output->setText(str);
			}
			else {

				output->setOutputDataType(DAT_OutDataType::Table);

				if (interChoice == 0) {

					output->setTableSize(totalRows, maxCols);
					int r = 0;
					int r2 = 0;
					int rowsAvailable = 0;
					int curRow = 0;

					for (int i = 0; i < maxRows; i++) {
						if (r < numRows) {
							for (int j = 0; j < cinput->numCols; j++)
							{
								const char* str = cinput->getCell(r, j);
								output->setCellString(curRow, j, str);
							}
							r++;
							curRow++;
						}

						if (r2 < numRows2) {
							for (int j = 0; j < cinput2->numCols; j++)
							{
								const char* str = cinput2->getCell(r2, j);
								output->setCellString(curRow, j, str);
							}
							r2++;
							curRow++;
						}

					}


				}
				else {

					output->setTableSize(maxRows, totalCols);
					int r = 0;
					int r2 = 0;
					int rowsAvailable = 0;
					int curRow = 0;

					for (int i = 0; i < maxCols; i++) {
						if (r < numCols) {
							for (int j = 0; j < cinput->numRows; j++)
							{
								const char* str = cinput->getCell(j, r);
								output->setCellString(j, curRow, str);
							}
							r++;
							curRow++;
						}

						if (r2 < numCols2) {
							for (int j = 0; j < cinput2->numRows; j++)
							{
								const char* str = cinput2->getCell(j, r2);
								output->setCellString(j, curRow, str);
							}
							r2++;
							curRow++;
						}

					}

				}
			}

		}
	}*/
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
	/*
	{
		OP_StringParameter	np;

		np.name = "Interleave";
		np.label = "Interleave";
		np.page = "Fold";

		const char* names[] = { "Interleaverows", "Interleavecols"};
		const char* labels[] = { "Interleave Rows", "Interleave Columns" };

		OP_ParAppendResult res = manager->appendMenu(np, 2, names, labels);
		assert(res == OP_ParAppendResult::Success);
	}*/
}

void
CPlusPlusDATExample::pulsePressed(const char* name, void* reserved1)
{
}
