#include "global.h"

const char *BOOL_STR(bool b)
{
	return b ? "true" : "false";
}

std::vector<std::string> split(std::string s, std::string delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

#if defined(__linux__)
void openFolderInExplorer(const char *path)
{
	system(("xdg-open \"" + std::string(path) + "\"").c_str());
}
std::string getDataFolder()
{
	std::string homedir = getenv("HOME");
	homedir.append("/.config/codename-launcher/");
	return homedir;
}
std::string detectConsoleEmulator()
{
	const char *terminals[] = {
	    "gnome-terminal", // GNOME Terminal
	    "xterm",	      // XTerm
	    "konsole",	      // Konsole (KDE)
	    "mate-terminal",  // MATE Terminal
	    "xfce4-terminal", // XFCE Terminal
	    "lxterminal",     // LXTerminal
	    "terminator",     // Terminator
	    "alacritty"	      // Alacritty
	};
	for (const char *terminal : terminals)
		if (system(("command -v " + std::string(terminal)).c_str()) == 0)
		{
			return std::string(terminal);
			break;
		}
}
void executeProgram(std::string path, std::string executable, bool allocateConsole, Engine *engine)
{
	pid_t pid = fork();

	if (pid == 0)
	{
		setsid();
		if (allocateConsole)
			executable = detectConsoleEmulator() + " -- " + executable;
		execlp("sh", "sh", "-c", ("cd \"" + path + "\" && " + executable).c_str(), (char *)NULL);
		return;
	}
	else
	{
		TraceLog(LOG_INFO, "detached");
		if (engine != nullptr)
		{
			engine->_processPid = pid;
		}
		// //TODO: make this async
		// int status;
		// pid_t result;
		// do {
		// 	result = waitpid(pid, &status, WNOHANG);
		// 	if (result == 0) sleep(1);
		// 	else if (WIFEXITED(status) || result != 0) {
		//     	TraceLog(LOG_INFO, "closed instance");
		// 		break;
		// 	}
		// }
		// while (result == 0);
	}
}
#elif defined(_WIN32)
#include "winapi.h"
void openFolderInExplorer(const char *path)
{
	system(("explorer \"" + std::string(path) + "\"").c_str());
}
std::string getDataFolder()
{
	std::string appdatadir = getenv("LOCALAPPDATA");
	appdatadir.append("\\CodenameLauncher");
	return appdatadir;
}
void executeProgram(std::string path, std::string executable, bool allocateConsole, Engine *engine)
{
	// TODO
}
#elif defined(__APPLE__)
void openFolderInExplorer(const char *path)
{
	system(("open \"" + std::string(path) + "\"").c_str());
}
std::string getDataFolder()
{
	std::string homedir = getenv("HOME");
	homedir.append("/Library/Application Support/Codename Launcher/");
	return homedir;
}
void executeProgram(std::string path, std::string executable, bool allocateConsole, Engine *engine)
{
	// TODO
}
#endif

int curCursorState = 1;

std::string getPath(std::string path)
{
	std::string gotPath = getDataFolder() + path;
	if (DirectoryExists(gotPath.c_str()))
		gotPath += "/";
	return gotPath;
}

std::string findExecutable(std::string searchPath)
{
	TraceLog(LOG_INFO, searchPath.c_str());
	const char *foundExecutable;
	FilePathList executableSearch;

	executableSearch = LoadDirectoryFilesEx(searchPath.c_str(), ".exe", false);
	foundExecutable = GetFileName(executableSearch.paths[0]);
#if defined(__linux__)
	executableSearch = LoadDirectoryFiles(searchPath.c_str());
	for (auto i = 0; i < (int)executableSearch.count; i++)
	{
		auto curpath = executableSearch.paths[i];
		if (std::string(GetFileName(curpath)) == std::string(GetFileNameWithoutExt(curpath)) && !DirectoryExists(curpath))
		{
			foundExecutable = GetFileName(curpath);
			break;
		}
	}
#elif defined(__APPLE__)
	// todo
	foundExecutable = "cum";
#endif

	TraceLog(LOG_INFO, foundExecutable);
	return foundExecutable;
}