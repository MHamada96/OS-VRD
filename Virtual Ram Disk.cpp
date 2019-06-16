#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

// directory includes
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

const int Mega = 1024 * 1024;
const int mxSize = 100 * Mega;
const int setWidth = 15;


int Space = mxSize;

int folderSize(string path)
{
	int sz = 0;
	DIR *dir;
	struct dirent *entry;
	struct stat info;

	dir = opendir(path.c_str());
	if (dir == NULL)
	{
		//cout << "Invalid Path" << endl;
	}
	else
	{
		while (true)
		{
			entry = readdir(dir);
			if (entry == NULL)
				break;

			string nm = entry->d_name;
			if (nm[0] == '.' || nm[0] == '$')
				continue;

			// if this is a directory => keep exploring
			if (entry->d_type == S_IFDIR)
			{
				sz += folderSize(path + "\\" + entry->d_name);
			}

			// is this is file ==> get the size
			else
			{
				FILE *F;

				F = fopen((path + "\\" + entry->d_name).c_str(), "rb");
				fseek(F, 0, SEEK_END);
				sz += ftell(F);
				rewind(F);
				fclose(F);
			}
		}
	}
	closedir(dir);
	return sz;
}

int myExplorFunction(string path)
{
	int sz = 0;

	DIR *dir;
	struct dirent *entry;
	struct stat info;

	dir = opendir(path.c_str());
	if (dir == NULL)
	{
		cout << "Invalid Path" << endl;
	}
	else
	{
		while (true)
		{
			entry = readdir(dir);
			if (entry == NULL)
				break;

			string nm = entry->d_name;
			if (nm[0] == '.' || nm[0] == '$')
				continue;

			cout << entry->d_name
				<< (entry->d_type == S_IFREG ? "  :==>file" : "  :==>Directory")
				<< endl;

			// if this is a directory => keep exploring
			if (entry->d_type == S_IFDIR)
			{
				sz += myExplorFunction(path + "\\" + entry->d_name);
			}

			// is this is file ==> get the size
			else
			{
				FILE *F;

				F = fopen((path + "\\" + entry->d_name).c_str(), "rb");
				fseek(F, 0, SEEK_END);
				sz += ftell(F);
				rewind(F);
				fclose(F);
			}
		}
	}

	closedir(dir);
	return sz;
}

struct _File_
{
	string name;

	// read the file content and store it here
	vector<char> Data;
};
struct _Folder_
{
	string name;
	_Folder_ *parent;
	vector<_Folder_*> inFolders; // vector of pointers fo folders
	vector<_File_*> inFilse;     // vector of pointers fo filse

	_Folder_()
	{
		name = "";
		parent = NULL;
	}
};


/*------------------------------------------*/
_Folder_ *Root;
_Folder_ *Location;



void helpMenu()
{
	cout << "\n---------------------\n\n";
	cout << setw(setWidth) << left << "help " << ": " << "Get the manula of the program" << endl;
	cout << setw(setWidth) << left << "md " << ": " << "Create Folder in the cuurent directory" << endl;
	cout << setw(setWidth) << left << "mf " << ": " << "Create file in the cuurent directory" << endl;
	cout << setw(setWidth) << left << "Stepout " << ": " << "Go to the parent Directory" << endl;
	cout << setw(setWidth) << left << "Stepin " << ": " << "Go to a specific child Directory(if Exist)" << endl;
	cout << setw(setWidth) << left << "Dir " << ": " << "show the content of the current Directory" << endl;
	cout << setw(setWidth) << left << "CopyDisk " << ": " << "Copy a Directory to the Virtual Disk" << endl;
	cout << setw(setWidth) << left << "Export " << ": " << "Export all the data to the " << endl;
	cout << setw(setWidth) << left << "Exit " << ": " << "End the program" << endl;
	cout << "\n---------------------\n\n";

}

void smallLetters(string &s)
{
	for (int i = 0; i < s.size(); i++)
		s[i] = tolower(s[i]);
}

string pathOf(_Folder_ *loc)
{
	if (loc->parent == NULL)
		return loc->name;

	return pathOf(loc->parent) + "\\" + loc->name;
}

bool createFolder(_Folder_ *curr, string fName, int sz = 0)
{
	bool ok = true;
	for (int i = 0; i < curr->inFolders.size(); i++)
	if (curr->inFolders[i]->name == fName)
		ok = false;

	if (ok)
	{
		_Folder_ *newFolder = new _Folder_;
		newFolder->parent = curr;
		newFolder->name = fName;
		curr->inFolders.push_back(newFolder);
		return true;
	}
	else
		return false;
}

bool createFile(_Folder_ *curr, string fName)
{
	bool ok = true;
	for (int i = 0; i < curr->inFilse.size(); i++)
	if (curr->inFilse[i]->name == fName)
		ok = false;

	if (ok)
	{
		_File_ *newFile = new _File_;
		newFile->name = fName;
		curr->inFilse.push_back(newFile);
		return true;
	}
	else
		return false;
}

void stepIn(string fName)
{
	int indx = -1;
	for (int i = 0; i < Location->inFolders.size(); i++)
	{
		if (Location->inFolders[i]->name == fName)
		{
			indx = i;
			break;
		}
	}
	bool ok = true;

	if (indx != -1)
		Location = Location->inFolders[indx];
	else
		ok = false;

	if (ok)
		cout << "got into the new Directory" << endl;
	else
		cout << "there is no such folder" << endl;
}

void stepOut()
{
	if (Location->parent == NULL)
		cout << "Faild ! , this Directory is the Root directory !" << endl;
	else
	{
		Location = Location->parent;
		cout << "Directory changed successfully" << endl;
	}
}

// show the content of the current Directory
void Dir(_Folder_*curr = Location)
{
	int sz = curr->inFolders.size() + curr->inFilse.size();
	if (sz == 0)
	{
		cout << "***Empty Directory***\n";
		return;
	}
	for (int i = 0; i < curr->inFolders.size(); i++)
		cout << curr->inFolders[i]->name << endl;
	cout << endl;

	for (int i = 0; i < curr->inFilse.size(); i++)
		cout << curr->inFilse[i]->name << endl;
}

void Copy(_Folder_ *curr, string path)
{

	int sz = 0;

	DIR *dir;
	struct dirent *entry;
	struct stat info;

	dir = opendir(path.c_str());
	if (dir == NULL)
	{
		//cout << "Invalid Path" << endl;
	}
	else
	{
		while (true)
		{
			entry = readdir(dir);
			if (entry == NULL)
				break;

			string nm = entry->d_name;
			if (nm[0] == '.' || nm[0] == '$')
				continue;

			// if this is a directory => keep exploring
			if (entry->d_type == S_IFDIR)
			{
				createFolder(curr, entry->d_name);
				Copy(curr->inFolders.back(), path + "\\" + entry->d_name);
			}

			// is this is file ==> get the size
			else
			{
				FILE *F;
				F = fopen((path + "\\" + entry->d_name).c_str(), "rb");
				fseek(F, 0, SEEK_END);
				sz += ftell(F);
				rewind(F);
				char *buffer = new char[sz];
				fread(buffer, 1, sz, F);
				fclose(F);

				createFile(curr, entry->d_name);
				curr->inFilse.back()->Data = vector<char>(buffer, buffer + sz);
			}
		}
	}

	closedir(dir);
}

void Export(_Folder_ *curr, string path)
{
	string com = "";
	com += "mkdir ";
	com += path;
	com += "\\";
	com += curr->name;


	system(com.c_str());
	string pcd = "cd ";
	pcd += path;
	pcd += "\\";
	pcd += curr->name;
	system(pcd.c_str());

	for (int i = 0; i < curr->inFilse.size(); i++)
	{
		string nm = curr->inFilse[i]->name;
		string comm = "";
		comm += path;
		comm += "\\";
		comm += curr->name;
		comm += "\\";
		comm += nm;

		FILE * pFile;
		pFile = fopen(comm.c_str(), "w");
		if (pFile != NULL)
		{
			int SZ = curr->inFilse[i]->Data.size();
			char *arr = new char[SZ];
			for (int j = 0; j < SZ; j++)
				arr[j] = curr->inFilse[i]->Data[j];
			fwrite(arr, 1, SZ, pFile);

			fclose(pFile);
		}
		//cout << "### " << comm << endl; Copy E:\ export
	}

	for (int i = 0; i < curr->inFolders.size(); i++)
	{
		if (curr->name.back() != '.')
			Export(curr->inFolders[i], path + "\\" + curr->name);
	}
}

int main()
{

	cout << myExplorFunction("E:\\");

	//cout << folderSize("E:\\"); //copy E:\ export
	// cout << folderSize("D:\\[FreeTutorials.Eu] Udemy - statistics-probability");

	string choice = "";

	Root = new _Folder_;
	Root->name = "RamDisk";

	Location = Root;

	while (true)
	{
		cout << pathOf(Location) + "> ";

		cin >> choice;
		smallLetters(choice);

		// help menu
		if (choice == "help")
		{
			helpMenu();
		}

		//show the path of the current DiskRam location
		else if (choice == "path")
		{
			cout << pathOf(Location) + "> " << endl;
		}

		//create a FOLDER inside the current location
		else if (choice == "md")
		{
			string folderName = "";
			cin >> folderName;
			bool ret = createFolder(Location, folderName);

			if (ret)
				cout << "Folder Created successfully" << endl;
			else
				cout << "Can't create the folder "
				<< folderName
				<< ", there's another folder with the same name"
				<< endl;
		}

		//create a FILE inside the current location
		else if (choice == "mf")
		{
			string fileName = "";
			cin >> fileName;
			bool ret = createFolder(Location, fileName);

			if (ret)
				cout << "File Created successfully" << endl;
			else
				cout << "Can't create the File "
				<< fileName
				<< ", there's another File with the same name"
				<< endl;
		}

		else if (choice == "stepin")
		{
			string childDir = "";
			cin >> childDir;
			stepIn(childDir);
		}
		else if (choice == "stepout")
		{
			stepOut();
		}
		else if (choice == "dir")
		{
			Dir();
		}
		else if (choice == "copy")
		{
			string p;
			cin >> p;
			int sz = folderSize(p);
			if (sz <= Space)
			{
				Space -= sz;
				Copy(Location, p);
			}
			else
			{
				cout << "This operation can't be done!"
					<< endl
					<< "File size : " << ceil(sz / (1024 * 1024 * 1.0)) << "MB , and the current Remain memory space is "
					<< Space / (1024 * 1024) << "MB" << endl;
			}
		}
		else if (choice == "export") // on Desktop by default
		{
			Export(Root, "C:\\Users\\MohamedHamada\\Desktop");
		}
		else if (choice == "exit")
		{
			return 0;
		}
		else
		{
			cout << "Undefined command" << endl;
		}
	}
	cin.get();
}