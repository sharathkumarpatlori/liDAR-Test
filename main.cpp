#include "main.h"

int main(int argc, char* argv[])
{

	std::string input;
	std::string output;
	std::string parse_string("xyz");
	std::string delimiter(",");
	std::ostream* os = NULL;

	bool verbose = false;
	bool output_xml = false;
	bool bPrintLabels = false;
	bool bPrintHeader = false;
	bool bUseStdout = false;

	std::vector<liblas::FilterPtr> filters;
	std::vector<liblas::TransformPtr> transforms;

	liblas::Header header;
	boost::array<boost::uint32_t, 4> precisions;
	precisions.assign(0);

	const char* parse_description = "The '--parse txyz' flag specifies how to format each"
		" each line of the ASCII file. For example, 'txyzia'"
		" means that the first number of each line should be the"
		" gpstime, the next three numbers should be the x, y, and"
		" z coordinate, the next number should be the intensity"
		" and the next number should be the scan angle.\n\n"
		" The supported entries are:\n"
		"   x - x coordinate as a double\n"
		"   y - y coordinate as a double\n"
		"   z - z coordinate as a double\n"
		"   X - x coordinate as unscaled integer\n"
		"   Y - y coordinate as unscaled integer\n"
		"   Z - z coordinate as unscaled integer\n"
		"   a - scan angle\n"
		"   i - intensity\n"
		"   n - number of returns for given pulse\n"
		"   r - number of this return\n"
		"   c - classification number\n"
		"   C - classification name\n"
		"   u - user data\n"
		"   p - point source ID\n"
		"   e - edge of flight line\n"
		"   d - direction of scan flag\n"
		"   R - red channel of RGB color\n"
		"   G - green channel of RGB color\n"
		"   B - blue channel of RGB color\n"
		"   M - vertex index number\n"
		;


	try {

		po::options_description file_options("las2txt options");
		po::options_description filtering_options = GetFilteringOptions();
		po::options_description header_options = GetHeaderOptions();

		po::positional_options_description p;
		p.add("input", 1);
		p.add("output", 1);


		file_options.add_options()
			("help,h", "produce help message")
			("input,i", po::value< string >(), "input LAS file.")
			("output,o", po::value< string >(), "output text file.  Use 'stdout' if you want it written to the standard output stream")
			("parse", po::value< string >(&parse_string), parse_description)
			("precision", po::value< std::vector<string> >()->multitoken(), "The number of decimal places to use for x,y,z,[t] output.  \n --precision 7 7 3\n --precision 3 3 4 6\nIf you don't specify any precision, las2txt uses the implicit values defined by the header's scale value (and a precision of 8 is used for any time values.)")
			("delimiter", po::value< string >(&delimiter), "The character to use for delimiting fields in the output.\n --delimiter \",\"\n --delimiter \"\t\"\n --delimiter \" \"")
			("labels", po::value<bool>(&bPrintLabels)->zero_tokens()->implicit_value(true), "Print row of header labels")
			("header", po::value<bool>(&bPrintHeader)->zero_tokens()->implicit_value(true), "Print header information")

			("verbose,v", po::value<bool>(&verbose)->zero_tokens(), "Verbose message output")
			("xml", po::value<bool>(&output_xml)->zero_tokens()->implicit_value(true), "Output as XML -- no formatting given by --parse is respected in this case.")
			("stdout", po::value<bool>(&bUseStdout)->zero_tokens()->implicit_value(true), "Output data to stdout")

			;

		po::variables_map vm;
		po::options_description options;
		options.add(file_options).add(filtering_options);
		po::store(po::command_line_parser(argc, argv).
			options(options).positional(p).run(), vm);

		po::notify(vm);

		if (vm.count("help"))
		{
			OutputHelp(std::cout, options);
			return 1;
		}


		if (vm.count("input"))
		{
			input = vm["input"].as< string >();
			std::ifstream ifs;
			if (verbose)
				std::cout << "Opening " << input << " to fetch Header" << std::endl;
			if (!liblas::Open(ifs, input.c_str()))
			{
				std::cerr << "Cannot open " << input << " for read.  Exiting..." << std::endl;
				return 1;
			}
			liblas::ReaderFactory f;
			liblas::Reader reader = f.CreateWithStream(ifs);
			header = reader.GetHeader();
		}
		else {
			std::cerr << "Input LAS file not specified!\n";
			OutputHelp(std::cout, options);
			return 1;
		}


		if (vm.count("output"))
		{
			output = vm["output"].as< string >();

			std::ios::openmode const mode = std::ios::out | std::ios::binary;
			if (compare_no_case(output.c_str(), "STDOUT", 5) == 0)
			{
				os = &std::cout;
				bUseStdout = true;
			}
			else
			{
				os = new std::ofstream(output.c_str(), mode);
			}

			if (!os->good())
			{
				delete os;
				std::cerr << "Cannot open " << output << " to write.  Exiting..." << std::endl;
				return 1;
			}



		}
		else {

			if (bUseStdout)
			{
				os = &std::cout;
			}
			else
			{
				std::cerr << "Output text file not specified!\n";
				OutputHelp(std::cout, options);
				return 1;
			}

		}
		filters = GetFilters(vm, verbose);

		std::ifstream ifs;
		if (!liblas::Open(ifs, input.c_str()))
		{
			std::cerr << "Cannot open " << input << " for read.  Exiting..." << std::endl;
			return -1;
		}

		if (vm.count("precision"))
		{
			std::vector<std::string> precision_str = vm["precision"].as< std::vector<std::string> >();
			if (precision_str.size() > 4) {
				ostringstream oss;
				oss << "Too many arguments were given to precision. "
					<< "--precision x y z [t]  or -- precision header  ";
				throw std::runtime_error(oss.str());
			}
			if (precision_str.size() < 3) {
				ostringstream oss;
				oss << "At least three arguments must be given to precision. ";
				throw std::runtime_error(oss.str());
			}

			if (verbose)
			{
				ostringstream oss;
				for (std::vector<std::string>::const_iterator i = precision_str.begin();
					i != precision_str.end();
					i++)
				{
					oss << *i << " ";
				}
				std::cout << "Setting precisions to: " << oss.str() << std::endl;
			}

			precisions[0] = boost::lexical_cast<boost::uint32_t>(precision_str[0]);
			precisions[1] = boost::lexical_cast<boost::uint32_t>(precision_str[1]);
			precisions[2] = boost::lexical_cast<boost::uint32_t>(precision_str[2]);

			if (precision_str.size() == 4)
			{
				precisions[3] = boost::lexical_cast<boost::uint32_t>(precision_str[3]);

			}
			else {
				precisions[3] = 8;
			}
		}
		else {

			precisions[0] = GetStreamPrecision(header.GetScaleX());
			precisions[1] = GetStreamPrecision(header.GetScaleY());
			precisions[2] = GetStreamPrecision(header.GetScaleZ());
			precisions[3] = 8;

			ostringstream oss;
			for (boost::array<boost::uint32_t, 4>::const_iterator i = precisions.begin();
				i != precisions.end();
				i++)
			{
				oss << *i << " ";
			}
			if (verbose)
			{
				std::cout << "Setting precisions from header to " << oss.str() << std::endl;
			}
		}

		if (vm.count("delimiter"))
		{
			std::string delim = vm["delimiter"].as< string >();
			std::string tab("\\t");
			std::string newline("\\n");
			if (!delim.compare(tab))
			{
				delimiter = "\t";
			}
			else if (!delim.compare(newline))
			{
				delimiter = "\n";
			}
			if (verbose)
			{
				std::cout << "Using delimiter '" << delim << "'" << std::endl;
			}

		}

		liblas::ReaderFactory f;
		liblas::Reader reader = f.CreateWithStream(ifs);

		write_points(reader,
			*os,
			parse_string,
			delimiter,
			filters,
			transforms,
			precisions,
			bPrintLabels,
			bPrintHeader,
			verbose);

		if (os != 0 && !bUseStdout)
			delete os;

	}
	catch (std::exception & e) {
		std::cerr << "error: " << e.what() << "\n";
		return 1;
	}
	catch (...) {
		std::cerr << "Exception of unknown type!\n";
	}

	return 0;


}