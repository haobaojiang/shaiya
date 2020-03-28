/**
* Author: Triston Plummer ("Cups")
* Date: 22/10/2016
*
* Used for decrypting Shen1l's XOR file count encryption for the SAH format.
*/
#include <fstream>
#include <iostream>
#include <vector>

// The encryption keys
int keys[4] = { 0x19, 0x68, 0x61, 0x74 };



bool g_IsDercrypt = false;

// A file structure
#pragma pack(push, 1)
struct File {

	// The amount of bytes in the files name
	uint32_t name_length;

	// The name of the file
	char* file_name;
	uint64_t offset;

	// The length of the files data
	uint64_t length;
};


// A folder structure
struct Folder {

	// The amount of bytes in the folder name
	uint32_t name_length;

	// The name of the folder
	char* folder_name;

	// The number of files in the folder
	uint32_t file_count;

	// The number of sub-directories contained within this folder
	uint32_t sub_folder_count;

	// The files contained within this folder
	std::vector<File> files;

	// The folders contained within this folder
	std::vector<Folder> folders;

	// The parent folder
	Folder* parent_folder;
};
#pragma pack(pop)

// Writes the folder and it's data to the output file
void write_folder(Folder* folder, std::ofstream* output);

// Reads a folder
void read_folder(Folder* folder, std::ifstream* input);



uint32_t crypto(uint32_t number_files_current_dir) {
	if (g_IsDercrypt) {
		for (int i = 4; i >0; i--) {
			number_files_current_dir ^= keys[i-1];
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			number_files_current_dir ^= keys[i];
		}
	}

	return number_files_current_dir;
}



int main(int argc, char** argv) {














	// A series of null bytes, used for writing
	char null_array[50] = { 0 };

	// The file instance
	std::ifstream* input = new std::ifstream((argc >= 2 ? argv[1] : "data.sah"), std::ifstream::in | std::ifstream::binary);

	// If the file couldn't be opened
	if (!input->is_open()) {
		std::cout << "Unable to open the file specified!" << std::endl;
		return 1;
	}

	// The file header
	char header[3];
	input->read((char*)&header, 3);

	// Skip the next 4 bytes
	input->ignore(4);

	// The total number of files in the header
	int total_number_files;

	// Read the total number of files
	input->read((char*)&total_number_files, 4);

	// Skip the next 45 bytes (we should now be at an offset of 56)
	input->ignore(45);

	// The root directory
	struct Folder root_directory;

	// Define the values for the root directory
	root_directory.folder_name = "Data\0";

	// Read the folder
	read_folder(&root_directory, input);

	// Close the header file
	input->close();

	// Write the decrypted header file
	std::ofstream* output = new std::ofstream((argc >= 3 ? argv[2] : "temp.bin"), std::ios::out | std::ios::binary);

	// If the file was opened
	if (!output->is_open()) {
		std::cout << "Error writing" << std::endl;
		return 1;
	}

	// Write the 3-byte header
	output->write("SAH", 3);

	// Write the 4 null bytes
	output->write((char*)null_array, 4);

	// Write the total number of files
	output->write((char*)&total_number_files, 4);

	// Write the 45 null bytes
	output->write((char*)null_array, 40);

	// Write a "1" followed by 4 null bytes
	output->write((char*) "\x01\0\0\0\0", 5);

	// Writes the root folder
	write_folder(&root_directory, output);

	// Append 8 null bytes
	output->write((char*)null_array, 8);

	// Close the output file
	output->close();
	return 0;
}

/**
* Recursively reads a specified folder
*
* [MENTION=1985011]param[/MENTION] folder
*		The folder to read
*
* [MENTION=1985011]param[/MENTION] input
*		The input file
*/
void read_folder(Folder* folder, std::ifstream* input) {

	// The number of files in the current directory
	int number_files_current_dir;
	input->read((char*)&number_files_current_dir, 4);

	// Decrypt the value

	number_files_current_dir = crypto(number_files_current_dir);


	// The number of files in the directory
	folder->file_count = number_files_current_dir;

	// Loop through the files in the current directory
	for (int i = 0; i < folder->file_count; i++) {

		// The current file being read
		struct File current_file;

		// The length of the file name
		input->read((char*)&current_file.name_length, 4);

		// The file name
		char* name = new char[current_file.name_length];
		input->read((char*)name, current_file.name_length);
		current_file.file_name = name;

		// The offset and length
		input->read((char*)&current_file.offset, 8);
		input->read((char*)&current_file.length, 8);

		// Add the file to the root directory instance
		folder->files.push_back(current_file);
	}

	// The number of sub-folders in the current directory
	int sub_folder_count;
	input->read((char*)&sub_folder_count, 4);
	folder->sub_folder_count = sub_folder_count;

	// Loop through the sub-directories
	for (int i = 0; i < folder->sub_folder_count; i++) {

		// The sub-directory
		struct Folder directory;

		// Define the directories parent folder
		directory.parent_folder = folder;

		// The length of the folder name
		input->read((char*)&directory.name_length, 4);

		// The folder name
		char* name = new char[directory.name_length];
		input->read((char*)name, directory.name_length);
		directory.folder_name = name;

		// Read the sub-directory
		read_folder(&directory, input);

		// Add the directory to the list of sub-folders
		folder->folders.push_back(directory);
	}
}

/**
* Recursively writes a specified folder to an output file
*
* [MENTION=1985011]param[/MENTION] folder
*		The folder to write
*
* [MENTION=1985011]param[/MENTION] output
*		The output file
*/
void write_folder(Folder* folder, std::ofstream* output) {

	// Write the number of files in the current folder
	output->write((char*)&folder->file_count, 4);

	// Loop through the files in the current directory, and write them
	for (auto &file : folder->files) {

		// The number of bytes in the file name
		output->write((char*)&file.name_length, 4);

		// The name of the file
		output->write((char*)file.file_name, file.name_length);

		// The offset and length of the file
		output->write((char*)&file.offset, 8);
		output->write((char*)&file.length, 8);
	}

	// Write the number of sub-directories in the current directory
	output->write((char*)&folder->sub_folder_count, 4);

	// Loop through the sub-directories
	for (auto &sub_folder : folder->folders) {

		// The number of bytes in the folder name
		output->write((char*)&sub_folder.name_length, 4);

		// The name of the folder
		output->write((char*)sub_folder.folder_name, sub_folder.name_length);

		// Write the current folder
		write_folder(&sub_folder, output);
	}
}