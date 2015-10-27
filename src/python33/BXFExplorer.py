
#!/usr/bin/python3
# utility for exploring and editing the contents of hercm matrix files

import readline
import libHercMatrix
import libHercmIO
import matplotlib
import matplotlib.pyplot
import logging
import os
import BXFUtils


def main(override=None):
    # override will be parsted instead of the user's input, if specified

    import pprint
    pp = pprint.PrettyPrinter()

    if override is None:
        usrIn = input("> ")
    else:
        usrIn = override

    usrIn = usrIn.rstrip()
    splitInput = usrIn.split()
    try:
        command = splitInput[0]
        arguments = splitInput[1:]
    except IndexError:
        return

    commandInfo = {}
    commandInfo['help'] = {'requiredArguments': None,
            'optionalArguments': [[0, str, 'command']],
            'argumentInfo': ['specific command to retrieve help for'],
            'help': """Prints help for all commands, or prints the help for the 
            command specified in the first argument"""}

    commandInfo['exit'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': 'exits the program'}

    commandInfo['load'] = {'requiredArguments': [[0, str, 'path'],
                    [1, str, 'format']],
        'optionalArguments': None,
        'argumentInfo': ['The file to load', 'The format of said file'],
        'help': 'Reads in the file for viewing and manipulation'}

    commandInfo['write'] = {'requiredArguments': [[0, str, 'path'],
                    [1, str, 'format']],
        'optionalArguments': None,
        'argumentInfo': ['The file to write to', 'The format of said file'],
        'help': """Writes current matrix to specified file, in specified format
         note that the given path should include the desired file extension"""}

    commandInfo['info'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': 'Prints information about the loaded matrix'}

    commandInfo['display'] = {'requiredArguments': None,
            'optionalArguments': [[0, int, 'height'], [1, int, 'width']],
            'argumentInfo': ['maximum number of elemets to display vertically',
                    'maximum number of elements to display horizontally'],
            'help': """Displays a visualization of the matrix. If the matrix is 
                    very large, only the corners will be displayed"""}

    commandInfo['csrdisplay'] = {'requiredArguments': None,
            'optionalArguments': [[0, int, 'rowStart'], [1, int, 'rowEnd']],
            'argumentInfo': ['first row to display', 'last row to display'],
            'help': """Displays the matrix as raw CSR data, prompts if nzentries 
            > 25.
             if provided, will only display the CSR values between a particular
             range of rows in the matrix"""}

    commandInfo['raw'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': 'display the raw COO format data for the matrix'}

    commandInfo['value'] = {'requiredArguments': [[0, int, 'column'],
            [1, int, 'row']],
        'optionalArguments': None,
        'argumentInfo': ['column of desired element', 'row of desired element'],
        'help': 'display the value at column, row'}

    commandInfo['row'] = {'requiredArguments': [[0, int, 'row']],
            'optionalArguments': None,
            'argumentInfo': ['the row to display'],
            'help': 'Displays all elements in the specified row'}

    commandInfo['col'] = {'requiredArguments': [[0, int, 'col']],
            'optionalArguments': None,
            'argumentInfo': ['the row to display'],
            'help': 'Displays all elements in the specified column'}

    commandInfo['range'] = {'requiredArguments': [[0, int, 'col1'],
                    [1, int, 'row1'],
        [2, int, 'col2'],
        [3, int, 'row2']],
        'optionalArguments': None,
        'argumentInfo': ['column of top-left corner',
                    'row of top-left corner',
                    'column of bottom-right corner',
                    'row of bottom-right corner'],
        'help': 'Displays all elements in the rectangular region given by ' +
                    '(row1, col1), (row2, col2)'}

    commandInfo['touch'] = {'requiredArguments': [[0, int, 'col'],
                    [1, int, 'row'],
        [2, float, 'val']],
        'optionalArguments': None,
        'argumentInfo': ['the column of the target element',
                    'the row of the target element',
                    'the new value for the element'],
        'help': 'Modifies the value of the matrix at the specified row and col'}

    commandInfo['paint'] = {'requiredArguments': [[0, int, 'col1'],
                    [1, int, 'row1'],
        [2, int, 'col2'],
        [3, int, 'row2']],
        'optionalArguments': [[0, float, 'val']],
        'argumentInfo': ['column of top-left corner',
                    'row of top-left corner',
                    'column of bottom-right corner',
                    'row of bottom-right corner',
                    'new value for elements'],
        'help': """Modifies the values of the rectangular range of elements 
        		whose top-left corner is (col1, row1) and whose bottom right 
        		corner is (col2, row2). If val is given, elements are set equal 
        		val, otherwise they are set to zero"""}

    commandInfo['paint-diag'] = {'requiredArguments': [[0, int, 'begin'],
                    [1, int, 'end'],
        [2, int, 'spread'],
        [3, float, 'val']],
        'optionalArguments': [[0, int, 'offset']],
        'argumentInfo': ['the first column of the diagonal',
                    'last column of the diagonal',
                    'number of indices on each side of the diagonal to paint',
                    'value to paint',
                    'number of indices to offset the diagonal horizontally'],
        'help': """sets all elements along the diagonal of the matrix to val, as
                well as spread values to either side of the diagonal, starting 
                column begin, and ending with column end. The diagonal can also 
                be offset by offset elements to the left or right """
    }

    commandInfo['row-major'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': """Makes the matrix row-major (only affects COO data, not
                    the contents of the matrix)"""}

    commandInfo['rmzeros'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': """Removes zero elements from the matrix (only affects COO
                    data not the contents of the matrix)"""}

    commandInfo['setdims'] = {'requiredArguments': [[0, int, 'with'],
                    [1, int, 'height']],
        'optionalArguments': None,
        'argumentInfo': ['the new width for the matrix',
                    'the new height for the matrix'],
        'help': 'Changes the dimensions of the matrix, truncating elements ' +
                    'which become out of bounds'}

    commandInfo['init'] = {'requiredArguments': [[0, int, 'with'],
                    [1, int, 'height']],
        'optionalArguments': [[0, float, 'val']],
        'argumentInfo': ['the width for the new matrix',
                    'the height for the new matrix',
                    'the value for all elements in the new matrix'],
        'help': """Creates a new matrix with specified dimensions, with all 
                elements initialized to zero, or to val if it is given"""}

    commandInfo['setsym'] = {'requiredArguments': [[0, str, 'symmetry']],
            'optionalArguments': [[0, str, 'method']],
            'argumentInfo': ['the new symmetry for the matrix',
                    'the algorithm to use'],
            'help': """ Makes the matrix symmetric or asymmetric, modifying COO 
            		data appropriately. By default, uses the truncate method. 
            		Available methods are: truncate - fastest, all elements from
            		the bottom triangle are removed/overwritten as needed; add -
            		all elements in in the lower triangle are added to 
            		corresponding elements in the upper triangle (asym->sym) OR 
            		all elements in the upper triangle are added to the 
            		corresponding elements in the lower (sym->asym); smart - 
            		only overwrites values with are zero, very slow"""}

    commandInfo['shell'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': 'Drop to Python debugging shell. WARNING: INTENDED FOR ' +
                    'DEBUGGING USE ONLY'}

    commandInfo['check-symmetry'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': """Checks the symmetry attribute of the matrix, and whether 
                    or not the data in the matrix is actually symmetrical"""}

    commandInfo['gen-verification'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': 'Updates the verification sum of the loaded matrix '}

    commandInfo['plot'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': 'Plots the matrix graphically with matplotlib'}

    commandInfo['transpose'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': 'Reflects the matrix about the diagonal'}

    commandInfo['ls'] = {'requiredArguments': None,
            'optionalArguments': [[0, str, "path"]],
            'argumentInfo': ["the path to get a listing for - default is ./"],
            'help': 'Prints a directory listing for the specified path'}

    commandInfo['pwd'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': 'Prints the current working directory'}

    commandInfo['head'] = {'requiredArguments': [[0, str, 'path']],
            'optionalArguments': [[0, int, 'lines']],
            'argumentInfo': ['the path to the file to get the head of',
                    'the number of lines to print from the file'],
            'help': 'Prints the first lines (10 by default) lines of the file'}

    commandInfo['cat'] = {'requiredArguments': [[0, str, 'path']],
            'optionalArguments': None,
            'argumentInfo': ['The file to print all lines from'],
            'help': 'Prints all lines from the file'}

    commandInfo['cd'] = {'requiredArguments': [[0, str, 'path']],
            'optionalArguments': None,
            'argumentInfo': ['new working directory'],
            'help': 'Changes the current working directory to path'}

    commandInfo['convert'] = {'requiredArguments': [[0, str, 'source'],
                    [1, str, 'source format'],
        [2, str, 'destination'],
        [3, str, 'destination format']],
        'optionalArguments': None,
        'argumentInfo': ['The path to the source file',
                    'the file format of the source file',
                    'the path to the destination file',
                    'the format of the destination file'],
        'help': """Reads the source file in the specified format, then writes it
                'back out at the specified destination in the destination 
                format"""}

    commandInfo['check-tril'] = {'requiredArguments': None,
            'optionalArguments': None,
            'argumentInfo': None,
            'help': 'Prints whether or not there are nonzero elements in the ' +
                    'lower triangle'}

    if command not in commandInfo:
        print("WARNING: command is not in commandInfo, cannot check required " +
              "arguments!")
    else:
        if commandInfo[command]['requiredArguments'] != None:
            if len(arguments) < len(commandInfo[command]['requiredArguments']):
                print("ERROR, incorrect number of arguments for {0}"
                      .format(command))
                firstMisingArgument = len(arguments)
                for i in range(firstMisingArgument,
                        len(commandInfo[command]['requiredArguments'])):
                    print("Missing argument '{1}' at position {0}".format(i,
                            commandInfo[command]['requiredArguments'][i][2]))
                return

            for arg in commandInfo[command]['requiredArguments']:
                try:
                    arguments[arg[0]] = arg[1](arguments[arg[0]])
                except Exception:
                    print("""ERROR: argument {0} was present, but is not of 
 required type {1}""".format(arg[0], str(arg[1])))
                    return

        if commandInfo[command]['optionalArguments'] != None:
            for arg in commandInfo[command]['optionalArguments']:
                argOffset = 0
                if commandInfo[command]['requiredArguments'] != None:
                    argOffset = len(commandInfo[command]['requiredArguments'])
                if argOffset + arg[0] > (len(arguments) - 1):
                    # this optional arg was not given
                    break

                try:
                    arguments[arg[0] + argOffset] = \
                        arg[1](arguments[arg[0] + argOffset])

                except IndexError:
                    print("WARNING: IndexError while accessing index {0} of {1}"
                          .format(arg[0] + argOffset, arguments))
                except Exception:
                    print("""ERROR: argument {0} was present, but of type {1} 
 not required type {2}""".format(arg[0] + argOffset,
                                 type(arguments[arg[0] + argOffset]),
                                 str(arg[1])))
                    return

    if command == 'help':
        if len(arguments) > 0:
            BXFUtils.printHelp(commandInfo, arguments[0])
        else:
            BXFUtils.printHelp(commandInfo)

    elif command == 'exit':
        exit()

    elif command == 'load':
        try:
            BXFUtils.load(arguments[0], arguments[1], SC)
        except AttributeError:
            print("ERROR: file does not exist")
            return
        except OSError:
            print("ERROR: file does not exist")
        except KeyError:
            print("ERROR: requested format is not supported")

        print("done reading matrix")
        if SC.HSM.symmetry == 'SYM':
            print("INFO: matrix is symmetric, bottom triangle should be only "
                  + "zeros")

    elif command == 'write':
        fileName = arguments[0]
        fileFormat = arguments[1]
        try:
            BXFUtils.write(fileName, fileFormat, SC)
        except FileExistsError:
            print("ERROR: file already exists!")
            return

    elif command == 'info':
        BXFUtils.printInfo(SC.HSM)

    elif command == 'display':
        if len(arguments) == 2:
            height = arguments[0]
            width = arguments[1]
            try:
                BXFUtils.displayMatrix(SC.HSM, height, width)
            except ValueError:
                print("ERROR: display dimensions must be even numbers!")
        else:
            BXFUtils.displayMatrix(SC.HSM)

    elif command == 'csrdisplay':
        if len(arguments) == 2:
            BXFUtils.printCSR(SC.HSM, arguments[0], arguments[1])
        else:
            BXFUtils.printCSR(SC.HSM)

    elif command == 'raw':
        BXFUtils.printRaw(SC.HSM)

    elif command == 'value':
        BXFUtils.printValue(arguments[0], arguments[1], SC.HSM)

    elif command == 'row':
        BXFUtils.printRow(arguments[0], SC.HSM)

    elif command == 'col':
        BXFUtils.printCol(arguments[0], SC.HSM)

    elif command == 'range':
        col1 = arguments[0]
        col2 = arguments[2]
        row1 = arguments[1]
        row2 = arguments[3]
        BXFUtils.printRange(row1, row2, col1, col2, SC.HSM)

    elif command == 'touch':
        BXFUtils.touch(arguments[0], arguments[1], arguments[2], SC.HSM)

    elif command == 'paint':
        val = 0.0
        col1 = arguments[0]
        row1 = arguments[1]
        col2 = arguments[2]
        row2 = arguments[3]
        if len(arguments) == 5:
            val = arguments[4]

        BXFUtils.paint(row1, row2, col1, col2, val, SC.HSM)

    elif command == 'paint-diag':

        begin = arguments[0]
        end = arguments[1]
        spread = arguments[2]
        val = arguments[3]
        offset = 0
        if len(arguments) == 5:
            offset = arguments[4]

        BXFUtils.paintDiagonal(begin, end, spread, val, SC.HSM, offset)

    elif command == 'row-major':
        print("making matrix row major, this may take some time, standby...")
        SC.HSM.makeRowMajor()
        print("done")

    elif command == 'rmzeros':
        print("removing zeros, standby...")
        SC.HSM.removeZeros()
        print("done")

    elif command == 'setdims':
        if arguments[1] > arguments[0]:
            # TODO: fix this
            print("WARNING: height is greater than width, this will probably " +
                  "break several commands (this is a known bug)")
        BXFUtils.setDims(arguments[1], arguments[0], SC.HSM)

    elif command == 'setsym':
        symmetry = arguments[0]
        method = 'truncate'
        if len(arguments) == 2:
            method = arguments[1]
        BXFUtils.setSymmetry(symmetry, SC.HSM, method)

    elif command == 'init':
        height = arguments[1]
        width = arguments[0]
        val = 0
        if len(arguments) == 3:
            val = arguments[2]
        BXFUtils.initilize(height, width, SC.HSM, val)

    elif command == 'shell':
        print("Entering python interactive debug shell...")
        print("Enter \"runMain()\" to return to normal execution")
        import pdb
        pdb.set_trace()

    elif command == 'check-symmetry':
        BXFUtils.printSymmetry(SC.HSM)

    elif command == 'gen-verification':
        BXFUtils.generateVerification(SC.HERCMIO, SC.HSM)

    elif command == 'plot':
        BXFUtils.plot(SC.HSM)

    elif command == "transpose":
        SC.HSM.transpose()

    elif command == "ls":
        directory = './'
        if len(arguments) == 1:
            directory = arguments[0]
        BXFUtils.printDirectoryListing(directory)

    elif command == "pwd":
        print(os.getcwd())

    elif command == "head":
        lines = 10
        path = arguments[0]
        if len(arguments) == 2:
            lines = arguments[1]

        BXFUtils.head(path, lines)

    elif command == "cat":
        BXFUtils.cat(arguments[0])

    elif command == "cd":
        BXFUtils.changeDirectory(arguments[0])

    elif command == "convert":
        BXFUtils.convert(
            arguments[0], arguments[2], arguments[1], arguments[3])

    elif command == 'check-tril':
        if SC.HSM.checkLowerTriangle():
            print("There are NO nonzero elements in the lower triangle")
        else:
            print("There ARE nonzero elements in the lower triangle")

    else:
        print("ERROR: Command not recognized")


def runMain():
    while True:
        main()

print("welcome to BXFExplorer. Enter \"help\" for help")
SC = libHercmIO.hercmIO()
runMain()
