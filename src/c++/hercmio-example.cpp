#include "hercmio.hpp"
#include <iomanip>

int main(int argc, char *argv[])
{

	int status = -1; 

	if (argc < 2) // make sure there is at least one argument 
	{
		cout << "FATAL: at least one argument is required" << endl;
		cout << "see ./hercmio-example help for help" 	   << endl;
	}

	string command = argv[1];
	if (command == "help")
	{
		cout << "hercmio-example usage:" 								<< endl;
		cout << "help   - - - - display this message"					<< endl;
		cout << "read [file]  - reads hercm file [file], prints as csr" << endl;
		cout << "write [file] - writes the following matrix to [file]"  << endl;
		cout << " 0 5 3 " 												<< endl;
		cout << " 2 9 4 " 												<< endl; 
		cout << " 0 0 1 " 												<< endl;
	}
	else if (command == "read")
	{
		if (argc != 3)
		{
			cout << "FATAL: incorrect number of arguments" << endl;
		}
		string inputFile = argv[2];
		cout << "reading file " << inputFile << endl;
		
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
		}

		cout << "Read header: "  	 			 << endl;
		cout << "width: " 		 << width		 << endl;
		cout << "height " 		 << height 		 << endl;
		cout << "nzentries: " 	 << nzentries	 << endl;
		cout << "symmetry: "	 << symmetry	 << endl;
		cout << "verification: " << verification << endl;

		cout << "reading matrix data..." << endl;
		cout << "allocating coo vectors..." << endl;
		float val[nzentries];
		int   col[nzentries];
		int   row[nzentries];

		cout << "reading data from file..." << endl;

		if (readHercm(inputFile, val, row, col) != HERCMIO_STATUS_SUCCESS)
		{
			cout << "FATAL: hercmio encountered error while reading the file";
			cout << endl;
		}

		cout << "read data in coo format: " << endl;
		cout << setprecision(6);
		cout << setw(10) << "val" << setw(10) << "col" << setw(10) << "row";
		cout << endl;
		cout << "------------------------------" << endl;
		for (int i = 0; i < nzentries; i++)
		{
			cout << setw(10) << val[i] << setw(10) << col[i] << setw(10);
			cout << row[i] << endl; 
		}

		cout << "calculated verification sum: ";
		cout << generateVerificationSum(row, col, val, nzentries) << endl;
		cout << "read sum from file: " << verification << endl;

		if (!verifyMatrix(inputFile, val, row, col))
		{
			cout << "WARNING: matrix verification does not match verification";
			cout << " sum read from file!" << endl;
		}
		else
		{
			cout << "verification sums match, verification passed" << endl;
		}

		cout << "making matrix row major..." << endl;
		makeRowMajor(row, col, val, nzentries);

		cout << "matrix in row major format: " << endl;
		cout << setprecision(6);
		cout << setw(10) << "val" << setw(10) << "col" << setw(10) << "row";
		cout << endl;
		cout << "------------------------------" << endl;
		for (int i = 0; i < nzentries; i++)
		{
			cout << setw(10) << val[i] << setw(10) << col[i] << setw(10);
			cout << row[i] << endl; 
		}

		cout << "converting matrix to csr format..." << endl;


		cout << "allocating ptr vector..." << endl; 
		int ptr[height];
		for (int i=0; i<height; i++)
		{
			ptr[i] = -999; 
		}


		cout << "performing conversion..." << endl;
		cooToCsr(row, col, val, ptr, nzentries, height);

		cout << "matrix in csr format:" << endl;
		cout << setw(10) << "val" << setw(10) << "col" << setw(10) << "ptr";
		cout << endl;
		cout << "------------------------------" << endl;

		int ptrCounter = 0;

		for (int i = 0; i < nzentries; i++)
		{
			cout << setw(10) << val[i] << setw(10) << col[i] << setw(10);
			if (ptrCounter < height)
			{
				cout << ptr[ptrCounter] << endl; 
				ptrCounter++;
			}
			else
			{
				cout << "N/A" << endl;
			}
		}

	}
	else if (command == "write")
	{
		if (argc != 3)
		{
			cout << "FATAL: incorrect number of arguments" << endl;
		}
		string outputFile = argv[2];
		cout << "writing the matrix: " 		 << endl;
		cout << " 0 5 3 " 			   		 << endl;
		cout << " 2 9 4 " 			   		 << endl; 
		cout << " 0 0 1 " 			   		 << endl;
		cout << "to the file " << outputFile << endl;
		cout << "matrix in COO format: "     << endl;
		cout << "VAL: 5 3 2 9 4 1"	 		 << endl;
		cout << "ROW: 0 0 1 1 1 2"			 << endl; 
		cout << "COL: 1 2 0 1 2 2"			 << endl;

		int col[6]   = {1,2,0,1,2,2};
		int row[6]   = {0,0,1,1,1,2};
		float val[6] = {5,3,2,9,4,1};
		int nzentries = 6;
		int width = 3;
		int height = 3; 
		string symmetry;
		symmetry = "ASYM";
		float verification;

		verification = generateVerificationSum(row, col, val, nzentries); 
		cout << "generated verification sum: " << verification << endl; 


		if (writeHercm(outputFile, 
					   height, 
					   width, 
					   nzentries, 
					   val, 
					   row, 
					   col, 
					   symmetry, 
					   verification) != HERCMIO_STATUS_SUCCESS)
		{
			cout << "FATAL: hercmio encountered an error while writing the";
			cout << " file" << endl; 
		}
		cout << "wrote file " << outputFile << endl;

	}
	


	return 0;
}