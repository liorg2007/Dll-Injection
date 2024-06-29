# Dll-Injection
Two scripts to generate a dll, and another to inject it into notepad.exe

Result: The dll(Any code you wish) is executed under the process of notepad.exe

## Usage
mydll2 is the project that creates the dll.

rundll runs the injection into notepad.exe.

To run the program you should build the mydll2 and take the path to the dll created.

Then build rundll, run the exe with the path as argument.

## How it works
The program waits for the target process.

Get address of "LoadLibraryA"  function which will run the dll.

Allocate memory in the remote thread using "VirtualAllocEx".

Write the dll path to the allocated memory using "WriteProcessMemory".

Then create a thread in the process that runs the dll using "CreateRemoteThread", where "LoadLibraryA" is the function and the dll path is the argument.

This leads for a thread inside the target program which runs the dll.
