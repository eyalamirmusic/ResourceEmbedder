#include <fstream>
#include <iostream>
#include <vector>

using Data = std::vector<unsigned char>;

template <typename T>
using Iterator = std::istreambuf_iterator<T>;

Data readDataFrom(const std::string& path)
{
    auto in = std::ifstream(path, std::ios::binary);

    if (!in)
        throw std::runtime_error("Error: cannot open input file: " + path);

    auto data = Data(Iterator(in), Iterator<char>());
    in.close();

    return data;
}

std::string getDirectory(const std::string& path)
{
    auto pos = path.find_last_of("/\\");

    if (pos != std::string::npos)
        return path.substr(0, pos);

    return ".";
}

std::string getFilename(const std::string& path)
{
    auto pos = path.find_last_of("/\\");

    if (pos != std::string::npos)
        return path.substr(pos + 1);

    return path;
}

std::vector<std::string> readFileList(const std::string& path)
{
    auto in = std::ifstream(path);

    if (!in)
        throw std::runtime_error("Error: cannot open file list: " + path);

    auto result = std::vector<std::string>();
    auto line = std::string();

    while (std::getline(in, line))
    {
        if (!line.empty())
            result.push_back(line);
    }

    return result;
}

struct EmbedArgs
{
    std::string inputFile;
    std::string outputC;
    int index;
};

struct InitArgs
{
    std::string outputDir;
    std::string namespaceName;
    std::string category;
    std::vector<std::string> inputFiles;
};

void writeDataFile(const std::string& input,
                   const std::string& output,
                   const std::string& varPrefix)
{
    auto data = readDataFrom(input);

    auto out = std::ofstream(output);

    if (!out)
        throw std::runtime_error("Error: cannot open output file: " + output);

    out << "const unsigned char " << varPrefix << "_data[] = {\n";

    for (size_t i = 0; i < data.size(); ++i)
    {
        if (i % 16 == 0)
            out << "    ";

        out << static_cast<unsigned int>(data[i]);

        if (i + 1 < data.size())
            out << ",";

        if (i % 16 == 15 || i + 1 == data.size())
            out << "\n";
        else
            out << " ";
    }

    out << "};\n\n";
    out << "const unsigned long " << varPrefix
        << "_size = sizeof(" << varPrefix << "_data);\n";

    if (!out)
    {
        throw std::runtime_error(
            "Error: failed to write output file: " + output);
    }
}

void writeEntriesCpp(const InitArgs& args)
{
    auto cppOutput = args.outputDir + "/" + args.namespaceName + ".cpp";
    auto out = std::ofstream(cppOutput);

    if (!out)
        throw std::runtime_error(
            "Error: cannot open output file: " + cppOutput);

    out << "#include \"ResourceEmbedLib.h\"\n\n";

    for (size_t i = 0; i < args.inputFiles.size(); ++i)
    {
        auto varPrefix = "resource_" + std::to_string(i);
        out << "extern const unsigned char " << varPrefix << "_data[];\n";
        out << "extern const unsigned long " << varPrefix << "_size;\n";
    }

    out << "\nnamespace " << args.namespaceName << "\n";
    out << "{\n";
    out << "const Resources::Entries& getResourceEntries()\n";
    out << "{\n";
    out << "    static const Resources::Entries entries = {\n";

    for (size_t i = 0; i < args.inputFiles.size(); ++i)
    {
        auto varPrefix = "resource_" + std::to_string(i);
        auto resourceName = getFilename(args.inputFiles[i]);
        out << "        {" << varPrefix << "_data, " << varPrefix << "_size, \""
            << resourceName << "\", \""
            << args.category << "\"}";

        if (i + 1 < args.inputFiles.size())
            out << ",";

        out << "\n";
    }

    out << "    };\n\n";
    out << "    return entries;\n";
    out << "}\n";
    out << "}\n";

    if (!out)
    {
        throw std::runtime_error(
            "Error: failed to write output file: " + cppOutput);
    }
}

void writeInitHeader(const InitArgs& args)
{
    auto headerOutput = args.outputDir + "/" + args.namespaceName + ".h";
    auto out = std::ofstream(headerOutput);

    if (!out)
        throw std::runtime_error(
            "Error: cannot open header file: " + headerOutput);

    out << "#pragma once\n\n";
    out << "#include \"ResourceEmbedLib.h\"\n\n";
    out << "namespace " << args.namespaceName << "\n";
    out << "{\n";
    out << "const Resources::Entries& getResourceEntries();\n";
    out << "static const Resources::Initializer resourceInitializer "
        << "{getResourceEntries()};\n";
    out << "}\n";

    if (!out)
    {
        throw std::runtime_error(
            "Error: failed to write header file: " + headerOutput);
    }
}

EmbedArgs getEmbedArgs(int argc, char* argv[])
{
    if (argc != 3)
    {
        throw std::runtime_error(
            "Usage: ResourceGenerator embed <input_file> <output.c> <index>");
    }

    return {argv[0], argv[1], std::stoi(argv[2])};
}

InitArgs getInitArgs(int argc, char* argv[])
{
    if (argc < 4)
    {
        throw std::runtime_error(
            "Usage: ResourceGenerator init <output_dir> <namespace> "
            "<category> <file_list>");
    }

    auto args = InitArgs();

    args.outputDir = argv[0];
    args.namespaceName = argv[1];
    args.category = argv[2];
    args.inputFiles = readFileList(argv[3]);

    if (args.inputFiles.empty())
        throw std::runtime_error("Error: no input files specified");

    return args;
}

void runEmbed(const EmbedArgs& args)
{
    auto varPrefix = "resource_" + std::to_string(args.index);
    writeDataFile(args.inputFile, args.outputC, varPrefix);
}

void runInit(const InitArgs& args)
{
    writeInitHeader(args);
    writeEntriesCpp(args);
}

std::string parseCommand(int argc, char* argv[])
{
    if (argc < 2)
    {
        throw std::runtime_error(
            "Usage: ResourceGenerator <embed|init> ...");
    }

    return {argv[1]};
}

void run(int argc, char* argv[])
{
    auto command = parseCommand(argc, argv);

    if (command == "embed")
    {
        auto args = getEmbedArgs(argc - 2, argv + 2);
        runEmbed(args);
    }
    else if (command == "init")
    {
        auto args = getInitArgs(argc - 2, argv + 2);
        runInit(args);
    }
    else
    {
        throw std::runtime_error(
            "Unknown command: " + command +
            "\nUsage: ResourceGenerator <embed|init> ...");
    }
}

int main(int argc, char* argv[])
{
    try
    {
        run(argc, argv);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}