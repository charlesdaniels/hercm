#include "hercmio.hpp"

bool checkVectorForString(vector<string> vectorToCheck, 
						  string stringToSearchFor)
{
	// checks of stringToSearchFor is contained by the vector of 
	//strings vectorToCheck. Returns true if it contains stringToSearchFor
	// is found at least once, and false if it is not found

	for (int i=0; i<vectorToCheck.size(); i++)
	{
		if (vectorToCheck[i] == stringToSearchFor)
		{
			return true;
		}
	}
	return false;
}

int stringToInt(string sourceString)
{
	// converts the string sourceString to an int, then returns that 
	int result; 
	stringstream convert(sourceString);
	if (!(convert >> result))
	{
		cout << "ERROR: failed to convert " << sourceString << " to integer";
		return -999999;
	}
	return result;

}

float stringToFloat(string sourceString)
{
	// converts the string sourceString to an float, then returns that 
	float result; 
	stringstream convert(sourceString);
	if (!(convert >> result))
	{
		cout << "ERROR: failed to convert " << sourceString << " to float";
		return -999999.0;
	}
	return result;

}

double stringToDouble(string sourceString)
{
	// converts the string sourceString to an double, then returns that 
	double result; 
	stringstream convert(sourceString);
	if (!(convert >> result))
	{
		cout << "ERROR: failed to convert " << sourceString << " to double";
		return -999999.0;
	}
	return result;

}


vector<string> split(string str, char delimiter) 
{
 	vector<string> internal;
  	stringstream ss(str); 
  	string tok;
  	
  	while(getline(ss, tok, delimiter)) 
  	{
  		if (tok != "")
  		{
			internal.push_back(tok);
		}
  	}
	return internal;
}

int readHercmHeader(string fileName, 
					int &width, 
					int &height, 
					int &nzentries, 
					string &symmetry,
					float &verification)
{
	// reads header of a hercm file
	// accepts header fields by reference, then populates values into those 
	
	// fileName - name of file to open
	// width - matrix width (number of cols)
	// height - matrix height (number of rows)
	// nzentries - number of nonzero entries in matrix
	// symmetry - matrix symmetry
	// verification - matrix verification sum as described in the hercm spec 

	// returns HERCMIO_STATUS_FAILURE on any error
	// returns HERCMIO_STATUS_SUCCESS otherwise

	string header; // this will store the header read from the file
	// the file object. c_str is required to make this work with c++98
	ifstream targetFile(fileName.c_str()); 

	if (targetFile.is_open())
	{
		getline(targetFile, header); // get the first line 
	}
	else
	{ 
		cout << "ERROR: Unable to open file " << fileName << endl;
		return HERCMIO_STATUS_FAILURE;
	}

	if (header.substr(0,5) != "BXF  " && 
		header.substr(0,5) != "HERCM" &&
		header.substr(0,3) != "BXF")
	{
		cout << "ERROR: malformed  or not a BXF file. read ";
		cout << header.substr(0,10) <<" expected HERCM or BXF" << endl;
		return HERCMIO_STATUS_FAILURE;
	}

	vector<string> headerItemsVector;
	headerItemsVector = split(header, ' ');

	if (headerItemsVector.size() != 6)
	{
		cout << "ERROR: malformed or not a BXF file. Expected 6 fields,";
		cout << " read ";
		cout << headerItemsVector.size() << " from " << header << endl;
		return HERCMIO_STATUS_FAILURE;
	}

	width        = stringToInt(headerItemsVector[1]);
	height       = stringToInt(headerItemsVector[2]);
	nzentries    = stringToInt(headerItemsVector[3]);
	symmetry     = headerItemsVector[4];
	verification = stringToFloat(headerItemsVector[5]);
	return HERCMIO_STATUS_SUCCESS; 

}

int readHercm(string fileName, float * val, int * row, int * col)
{
	// reads the file contents of the hercm file, then populates val, row_ptr,
	// and colind with the values thereof. 

	// fileName - the name of the hercm file to read 
	// val - the val vector for the matrix
	// row - the row vector for the matrix
	// col - the col vector for the matrix 

	// returns HERCMIO_STATUS_FAILURE on any error
	// returns HERCMIO_STATUS_SUCCESS otherwise

	string line; // this will store each line of the file as we read it in
	int lineCounter=0; // we will use this to keep track of what line we are on
	ifstream targetFile(fileName.c_str()); // instantiate the file object
	// c_str is required pre-c++11 as ifstream expects a const char* 

 
	string currentField; // this will store the last read field
	vector<string> splitLine; 

	// keep track of our position in row, col, and val
	int masterValCounter = 0;
	int masterColCounter = 0;
	int masterRowCounter = 0;

	// variables needed to read headers 
	int width;
	int height;
	int nzentries;
	string symmetry;
	float verification; 

	if (readHercmHeader(fileName, 
					   width, 
					   height, 
					   nzentries, 
					   symmetry, 
					   verification) != HERCMIO_STATUS_SUCCESS)
	{
		cout << "ERROR: could not read header" << endl;
		return HERCMIO_STATUS_FAILURE;
	}
	
	if (targetFile.is_open()) // check if opening the file failed
	{
		while ( getline (targetFile, line)) // while we can get a new line 
		{
			lineCounter++;
			if (lineCounter == 1)
			{
				// we need to make sure this is a HeRCM file
				if (line.substr(0,5) != "HERCM")
				{
					cout<<"ERROR: malformed file or not hercm format (expected";
					cout << " \"HERCM\", found \""<<line<<"\")"<<endl;
					return HERCMIO_STATUS_FAILURE;
				}
			}
			
			
			splitLine = split(line, ' '); 


			// this is a bit ugly, but it prevents us from having to use
			// std::find
			if (splitLine[0] == "VAL")
			{
				currentField = "VAL";
			}
			else if (splitLine[0] == "ROW")
			{
				currentField = "ROW"; 
			}
			else if (splitLine[0] == "COL")
			{
				currentField = "COL";
			}
			else if (splitLine[0] == "ENDFIELD")
			{
				currentField = "";
			}
			else
			{
				if (currentField == "VAL")
					for (int i = 0; i < splitLine.size(); i++)
					{
						val[masterValCounter] = stringToFloat(splitLine[i]);
						masterValCounter++;
					}
				if (currentField == "ROW")
					for (int i = 0; i < splitLine.size(); i++)
					{
						row[masterRowCounter] = stringToInt(splitLine[i]);
						masterRowCounter++;
					}
				if (currentField == "COL")
					for (int i = 0; i < splitLine.size(); i++)
					{
						col[masterColCounter] = stringToInt(splitLine[i]);
						masterColCounter++;
					}
			}
			
		}
	}
	else
	{ 
		cout << "ERROR: Unable to open file " << fileName << endl;
		return HERCMIO_STATUS_FAILURE;
	}

	return HERCMIO_STATUS_SUCCESS;
}

int readHercm(string fileName, double * val, int * row, int * col)
{
	// reads the file contents of the hercm file, then populates val, row_ptr,
	// and colind with the values thereof. 

	// fileName - the name of the hercm file to read 
	// val - the val vector for the matrix
	// row - the row vector for the matrix
	// col - the col vector for the matrix 

	// returns HERCMIO_STATUS_FAILURE on any error
	// returns HERCMIO_STATUS_SUCCESS otherwise

	string line; // this will store each line of the file as we read it in
	int lineCounter=0; // we will use this to keep track of what line we are on
	ifstream targetFile(fileName.c_str()); // instantiate the file object
	// c_str is required pre-c++11 as ifstream expects a const char* 

 
	string currentField; // this will store the last read field
	vector<string> splitLine; 

	// keep track of our position in row, col, and val
	int masterValCounter = 0;
	int masterColCounter = 0;
	int masterRowCounter = 0;

	// variables needed to read headers 
	int width;
	int height;
	int nzentries;
	string symmetry;
	float verification; 

	if (readHercmHeader(fileName, 
					   width, 
					   height, 
					   nzentries, 
					   symmetry, 
					   verification) != HERCMIO_STATUS_SUCCESS)
	{
		cout << "ERROR: could not read header" << endl;
		return HERCMIO_STATUS_FAILURE;
	}
	
	if (targetFile.is_open()) // check if opening the file failed
	{
		while ( getline (targetFile, line)) // while we can get a new line 
		{
			lineCounter++;
			if (lineCounter == 1)
			{
				// we need to make sure this is a HeRCM file
				if (line.substr(0,5) != "HERCM")
				{
					cout<<"ERROR: malformed file or not hercm format (expected";
					cout << " \"HERCM\", found \""<<line<<"\")"<<endl;
					return HERCMIO_STATUS_FAILURE;
				}
			}
			
			
			splitLine = split(line, ' '); 


			// this is a bit ugly, but it prevents us from having to use
			// std::find
			if (splitLine[0] == "VAL")
			{
				currentField = "VAL";
			}
			else if (splitLine[0] == "ROW")
			{
				currentField = "ROW"; 
			}
			else if (splitLine[0] == "COL")
			{
				currentField = "COL";
			}
			else if (splitLine[0] == "ENDFIELD")
			{
				currentField = "";
			}
			else
			{
				if (currentField == "VAL")
					for (int i = 0; i < splitLine.size(); i++)
					{
						val[masterValCounter] = stringToFloat(splitLine[i]);
						masterValCounter++;
					}
				if (currentField == "ROW")
					for (int i = 0; i < splitLine.size(); i++)
					{
						row[masterRowCounter] = stringToInt(splitLine[i]);
						masterRowCounter++;
					}
				if (currentField == "COL")
					for (int i = 0; i < splitLine.size(); i++)
					{
						col[masterColCounter] = stringToInt(splitLine[i]);
						masterColCounter++;
					}
			}
			
		}
	}
	else
	{ 
		cout << "ERROR: Unable to open file " << fileName << endl;
		return HERCMIO_STATUS_FAILURE;
	}

	return HERCMIO_STATUS_SUCCESS;
}

bool checkIfSorted(int * vector, int size)
{
	// vector is a 1-d matrix to check 
	// size is it's number of elements

	// returns true if array is sorted from least to greatest
	// returns false otherwise 

	for (int i=1; i<size; i++)
	{
		if (vector[i-1] > vector[i])
		{
			return false; 
		}
	}
	return true; 

}

bool checkIfRowMajor(int * row, int * col, int nzentries)
{
	// returns true of the matrix is row major, false otherwise
	// same calling convention as makeRowMajor

	if (!checkIfSorted(row, nzentries))
	{
		return false;
	}
	for (int i=1; i<nzentries; i++)
	{
		if ((col[i-1] > col[i]) && (row[i-1] == row[i])) // see makeRowMajor 
		{
			return false; 
		}
	}
	return true;
}


int makeRowMajor(int * row, int * col, float * val, int nzentries)
{
	// row, col, and val should be coo vectors, as laid out in the HeRCM spec
	// nzentries should be the number of elements in val 

	// always returns HERCMIO_STATUS_SUCCESS

	while (!checkIfSorted(row, nzentries))
	{
		for (int i=1; i<nzentries; i++)
		{
			if (row[i-1] > row[i])
			{
				// swap row values
				int smallerValue = row[i];
				int largerValue  = row[i-1];
				row[i] = largerValue; 
				row[i-1] = smallerValue;

				// swap values in col to match
				smallerValue = col[i];
				largerValue  = col[i-1];
				col[i] = largerValue; 
				col[i-1] = smallerValue; 

				// swap values in val to match
				float smallerValueFloat = val[i];
				float largerValueFloat  = val[i-1];
				val[i] = largerValueFloat; 
				val[i-1] = smallerValueFloat; 
			}
		}
	}

	while (!checkIfRowMajor(row, col, nzentries))
	{
		for (int i=1; i<nzentries; i++)
		{
			// check if the previous value of col is large than the current
			// value of col AND the row value for both is identical 
			if ((col[i-1] > col[i]) && (row[i] == row[i-1]))
			{
				// swap row values
				int smallerValue = row[i];
				int largerValue  = row[i-1];
				row[i] = largerValue; 
				row[i-1] = smallerValue;
				// swap values in col to match
				smallerValue = col[i];
				largerValue  = col[i-1];
				col[i] = largerValue; 
				col[i-1] = smallerValue; 
				// swap values in val to match
				float smallerValueFloat = val[i];
				float largerValueFloat  = val[i-1];
				val[i] = largerValueFloat; 
				val[i-1] = smallerValueFloat; 
			}
		}
	}

	return HERCMIO_STATUS_SUCCESS;
}

int makeRowMajor(int * row, int * col, double * val, int nzentries)
{
	// row, col, and val should be coo vectors, as laid out in the HeRCM spec
	// nzentries should be the number of elements in val 

	// always returns HERCMIO_STATUS_SUCCESS

	while (!checkIfSorted(row, nzentries))
	{
		for (int i=1; i<nzentries; i++)
		{
			if (row[i-1] > row[i])
			{
				// swap row values
				int smallerValue = row[i];
				int largerValue  = row[i-1];
				row[i] = largerValue; 
				row[i-1] = smallerValue;

				// swap values in col to match
				smallerValue = col[i];
				largerValue  = col[i-1];
				col[i] = largerValue; 
				col[i-1] = smallerValue; 

				// swap values in val to match
				double smallerValueFloat = val[i];
				double largerValueFloat  = val[i-1];
				val[i] = largerValueFloat; 
				val[i-1] = smallerValueFloat; 
			}
		}
	}

	while (!checkIfRowMajor(row, col, nzentries))
	{
		for (int i=1; i<nzentries; i++)
		{
			// check if the previous value of col is large than the current
			// value of col AND the row value for both is identical 
			if ((col[i-1] > col[i]) && (row[i] == row[i-1]))
			{
				// swap row values
				int smallerValue = row[i];
				int largerValue  = row[i-1];
				row[i] = largerValue; 
				row[i-1] = smallerValue;
				// swap values in col to match
				smallerValue = col[i];
				largerValue  = col[i-1];
				col[i] = largerValue; 
				col[i-1] = smallerValue; 
				// swap values in val to match
				double smallerValueFloat = val[i];
				double largerValueFloat  = val[i-1];
				val[i] = largerValueFloat; 
				val[i-1] = smallerValueFloat; 
			}
		}
	}

	return HERCMIO_STATUS_SUCCESS;
}


int cooToCsr(int * row, 
			 int * col, 
			 float * val, 
			 int * ptr, 
			 int nzentries, 
			 int height)
{
	// converts coo matrix given by row, col, and val to csr
	// ptr is the row_ptr array for CSR, allocated to be height long

	int currentRow = -1; // make sure the first row is accounted for
	int ptrCounter = 0; // keep track of where we are in ptr

	makeRowMajor(row, col, val, nzentries); // make sure the matrix is already
	// row major 

	for (int i=0; i<nzentries; i++)
	{
		if (row[i] != currentRow) // check if this is the start of a new row
		{
			currentRow = row[i];
			ptr[ptrCounter] = i; 
			ptrCounter++;
		}
	}
	ptr[ptrCounter] = nzentries+1;
	return HERCMIO_STATUS_SUCCESS;
}

int cooToCsr(int * row, 
			 int * col, 
			 double * val, 
			 int * ptr, 
			 int nzentries, 
			 int height)
{
	// converts coo matrix given by row, col, and val to csr
	// ptr is the row_ptr array for CSR, allocated to be height long

	int currentRow = -1; // make sure the first row is accounted for
	int ptrCounter = 0; // keep track of where we are in ptr

	makeRowMajor(row, col, val, nzentries); // make sure the matrix is already
	// row major 

	for (int i=0; i<nzentries; i++)
	{
		if (row[i] != currentRow) // check if this is the start of a new row
		{
			currentRow = row[i];
			ptr[ptrCounter] = i; 
			ptrCounter++;
		}
	}
	ptr[ptrCounter] = nzentries+1;
	return HERCMIO_STATUS_SUCCESS;
}

float generateVerificationSum(int * row, int * col, float * val, int nzentries)
{
	// same calling convention as makeRowMajor() 
	// returns the verification sum for the matrix, as described in the HeRCM
	// spec as a float 

	float sum = 0;
	for (int i=0; i<nzentries;i++)
	{
		sum = sum + (1.0* row[i]) + (1.0 * col[i]) + val[i];
	}
	return fmod(sum,(1.0 * nzentries)); 
}

float generateVerificationSum(int * row, int * col, double * val, int nzentries)
{
	// same calling convention as makeRowMajor() 
	// returns the verification sum for the matrix, as described in the HeRCM
	// spec as a float 

	float sum = 0;
	for (int i=0; i<nzentries;i++)
	{
		sum = sum + (1.0* row[i]) + (1.0 * col[i]) + val[i];
	}
	return fmod(sum,(1.0 * nzentries)); 
}

bool verifyMatrix(string inputFile, float * val, int * row, int * col)
{
	// same calling convention as readHercm() 
	// returns true is the matrix's verification sum matches the one in it's
	// header 

	// variables needed to read headers 
	int width;
	int height;
	int nzentries;
	string symmetry;
	float verification; 
	if (readHercmHeader(inputFile, 
					   width, 
					   height, 
					   nzentries, 
					   symmetry, 
					   verification) != HERCMIO_STATUS_SUCCESS)
	{
		cout << "FATAL: hercmio encountered an error while reading";
		cout << " the header!" << endl;
		return false;
	}

	if (verification == generateVerificationSum(row, col, val, nzentries))
	{
		return true;
	}
	return false;
}

bool verifyMatrix(string inputFile, double * val, int * row, int * col)
{
	// same calling convention as readHercm() 
	// returns true is the matrix's verification sum matches the one in it's
	// header 

	// variables needed to read headers 
	int width;
	int height;
	int nzentries;
	string symmetry;
	float verification; 
	if (readHercmHeader(inputFile, 
					   width, 
					   height, 
					   nzentries, 
					   symmetry, 
					   verification) != HERCMIO_STATUS_SUCCESS)
	{
		cout << "FATAL: hercmio encountered an error while reading";
		cout << " the header!" << endl;
		return false;
	}

	if (verification == generateVerificationSum(row, col, val, nzentries))
	{
		return true;
	}
	return false;
}

int writeHercm(string fileName, 
			   int height, 
			   int width, 
			   int nzentries, 
			   float * val, 
			   int * row, 
			   int * col, 
			   string symmetry, 
			   float verification)
{
	// writes a CSR matrix to a file 

	// fileName - name of file for writing
	// height - height of matrix (number of rows)
	// width - width of matrix (number of cols)
	// nzentries - number of non zero entries in matrix
	// val - CSR val array
	// row_ptr - CSR row_ptr array
	// colind - CSR colind array 
	// symmetry - symmetry of matrix, either SYM or ASYM per hercm spec
	// verification - verification sum as described in the HeRCM spec

	// returns HERCMIO_STATUS_FAILURE on failure
	// returns HERCMIO_STATUS_SUCCESS otherwise

	makeRowMajor(row, col, val, nzentries); // make sure we are in row major
	// format 

	ofstream targetFile (fileName.c_str());
	string header; // the header 

	if (!targetFile.is_open())
	{
		cout <<"ERROR: could not open file " << fileName << endl;
		return HERCMIO_STATUS_FAILURE;
	}

	if (symmetry != "SYM" && symmetry != "ASYM")
	{
		cout << "ERROR: illegal value for symmetry, expected SYM or ASYM, got ";
		cout << symmetry << endl;
		return HERCMIO_STATUS_FAILURE;
	}

	header = "HERCM ";
	// these are the easiest ways to cast ints and floats to string
	header.append(static_cast<ostringstream*>( 
				  &(ostringstream() << width) )->str()); 
	header.append(" ");
	header.append(static_cast<ostringstream*>( 
				  &(ostringstream() << height) )->str());
	header.append(" ");
	header.append(static_cast<ostringstream*>( 
				  &(ostringstream() << nzentries) )->str());
	header.append(" ");
	header.append(symmetry);
	header.append(" ");
	header.append(static_cast<ostringstream*>( 
				  &(ostringstream() << verification))->str());

	targetFile << header <<endl; // write out the header we just generated

	targetFile << "REMARKS LIST STRING" << endl; 
	targetFile << "ENDFIELD" << endl; 
	
	targetFile << "VAL LIST FLOAT" << endl; 
	int lineCounter = 0; // count our position in the line so we can insert 
	// newlines as needed 
	for (int i=0; i<nzentries; i++)
	{

		targetFile << val[i]; 
		if (lineCounter == 9) 
		{
			targetFile << endl;
			lineCounter = 0;
		}
		else
		{
			targetFile << ' ';
		}

	}
	targetFile << endl; 
	
	targetFile << "ENDFIELD" << endl;

	targetFile << "ROW LIST INT" << endl; 
	lineCounter = 0; // count our position in the line so we can insert 
	// newlines as needed 
	for (int i=0; i<nzentries; i++)
	{

		targetFile << row[i]; 
		if (lineCounter == 9) 
		{
			targetFile << endl;
			lineCounter = 0;
		}
		else
		{
			targetFile << ' ';
		}

	}
	targetFile << endl; 
	targetFile << "ENDFIELD" << endl;

	targetFile << "COL LIST INT" << endl; 
	lineCounter = 0; // count our position in the line so we can insert 
	// newlines as needed 
	for (int i=0; i<nzentries; i++)
	{

		targetFile << col[i]; 
		if (lineCounter == 9) 
		{
			targetFile << endl;
			lineCounter = 0;
		}
		else
		{
			targetFile << ' ';
		}

	}
	
	targetFile << endl; 
	
	targetFile << "ENDFIELD" << endl;

	
	return HERCMIO_STATUS_SUCCESS;

}

int writeHercm(string fileName, 
			   int height, 
			   int width, 
			   int nzentries, 
			   double * val, 
			   int * row, 
			   int * col, 
			   string symmetry, 
			   float verification)
{
	// writes a CSR matrix to a file 

	// fileName - name of file for writing
	// height - height of matrix (number of rows)
	// width - width of matrix (number of cols)
	// nzentries - number of non zero entries in matrix
	// val - CSR val array
	// row_ptr - CSR row_ptr array
	// colind - CSR colind array 
	// symmetry - symmetry of matrix, either SYM or ASYM per hercm spec
	// verification - verification sum as described in the HeRCM spec

	// returns HERCMIO_STATUS_FAILURE on failure
	// returns HERCMIO_STATUS_SUCCESS otherwise

	makeRowMajor(row, col, val, nzentries); // make sure we are in row major
	// format 

	ofstream targetFile (fileName.c_str());
	string header; // the header 

	if (!targetFile.is_open())
	{
		cout <<"ERROR: could not open file " << fileName << endl;
		return HERCMIO_STATUS_FAILURE;
	}

	if (symmetry != "SYM" && symmetry != "ASYM")
	{
		cout << "ERROR: illegal value for symmetry, expected SYM or ASYM, got ";
		cout << symmetry << endl;
		return HERCMIO_STATUS_FAILURE;
	}

	header = "BXF   ";
	// these are the easiest ways to cast ints and floats to string
	header.append(static_cast<ostringstream*>( 
				  &(ostringstream() << width) )->str()); 
	header.append(" ");
	header.append(static_cast<ostringstream*>( 
				  &(ostringstream() << height) )->str());
	header.append(" ");
	header.append(static_cast<ostringstream*>( 
				  &(ostringstream() << nzentries) )->str());
	header.append(" ");
	header.append(symmetry);
	header.append(" ");
	header.append(static_cast<ostringstream*>( 
				  &(ostringstream() << verification))->str());

	targetFile << header <<endl; // write out the header we just generated

	targetFile << "REMARKS LIST STRING" << endl; 
	targetFile << "ENDFIELD" << endl; 
	
	targetFile << "VAL LIST FLOAT" << endl; 
	int lineCounter = 0; // count our position in the line so we can insert 
	// newlines as needed 
	for (int i=0; i<nzentries; i++)
	{

		targetFile << val[i]; 
		if (lineCounter == 9) 
		{
			targetFile << endl;
			lineCounter = 0;
		}
		else
		{
			targetFile << ' ';
		}

	}
	targetFile << endl; 
	
	targetFile << "ENDFIELD" << endl;

	targetFile << "ROW LIST INT" << endl; 
	lineCounter = 0; // count our position in the line so we can insert 
	// newlines as needed 
	for (int i=0; i<nzentries; i++)
	{

		targetFile << row[i]; 
		if (lineCounter == 9) 
		{
			targetFile << endl;
			lineCounter = 0;
		}
		else
		{
			targetFile << ' ';
		}

	}
	targetFile << endl; 
	targetFile << "ENDFIELD" << endl;

	targetFile << "COL LIST INT" << endl; 
	lineCounter = 0; // count our position in the line so we can insert 
	// newlines as needed 
	for (int i=0; i<nzentries; i++)
	{

		targetFile << col[i]; 
		if (lineCounter == 9) 
		{
			targetFile << endl;
			lineCounter = 0;
		}
		else
		{
			targetFile << ' ';
		}

	}
	
	targetFile << endl; 
	
	targetFile << "ENDFIELD" << endl;

	
	return HERCMIO_STATUS_SUCCESS;

}
