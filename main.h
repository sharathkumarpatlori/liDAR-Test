
#include <liblas/liblas.hpp>
#include "laskernel.hpp"
#include <liblas/utility.hpp>

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

using namespace liblas;
using namespace std;

#ifdef _WIN32
#define compare_no_case(a,b,n)  _strnicmp( (a), (b), (n) )
#else
#define compare_no_case(a,b,n)  strncasecmp( (a), (b), (n) )
#endif


std::string GetLabels(std::string const& parse_string,
	std::string const& delimiter)
{
	std::ostringstream output;

	boost::uint32_t i = 0;
	for (;;) //infinite loop
	{

		switch (parse_string[i])
		{
			/* // the x coordinate */
		case 'x':
			output << "\"X\"";
			break;
			/* // the y coordinate */
		case 'y':
			output << "\"Y\"";
			break;
			/* // the z coordinate */
		case 'z':
			output << "\"Z\"";
			break;
		

		i++;

		if (parse_string[i])
		{
			output << delimiter;
		}
		else
		{
			output << std::endl;
			break;
		}

	}
	return output.str();
}

std::string GetPointString(std::string const& parse_string,
	std::string const& delimiter,
	liblas::Point const& p,
	boost::array<boost::uint32_t, 4> precisions,
	boost::uint32_t index)
{

	std::ostringstream output;

	boost::uint32_t i = 0;
	liblas::Color const& c = p.GetColor();
	for (;;) //Infinite loop
	{

		switch (parse_string[i])
		{
			/* // the x coordinate */
		case 'x':
			output.setf(std::ios_base::fixed, std::ios_base::floatfield);
			output.precision(precisions[0]); //x precision
			output << p.GetX();
			output.unsetf(std::ios_base::fixed);
			output.unsetf(std::ios_base::floatfield);
			break;
			/* // the y coordinate */
		case 'y':
			output.setf(std::ios_base::fixed, std::ios_base::floatfield);
			output.precision(precisions[1]); //y precision
			output << p.GetY();
			output.unsetf(std::ios_base::fixed);
			output.unsetf(std::ios_base::floatfield);
			break;
			/* // the z coordinate */
		case 'z':
			output.setf(std::ios_base::fixed, std::ios_base::floatfield);
			output.precision(precisions[2]); //z precision
			output << p.GetZ();
			output.unsetf(std::ios_base::fixed);
			output.unsetf(std::ios_base::floatfield);
			break;
			

		i++;

		if (parse_string[i])
		{
			output << delimiter;
		}
		else
		{
			output << std::endl;
			break;
		}

	}


	return output.str();
}

std::string GetHeader(liblas::Reader& reader)
{
	boost::ignore_unused_variable_warning(reader);

	std::ostringstream oss;

	oss << reader.GetHeader();

	return oss.str();
}
void write_points(liblas::Reader& reader,
	std::ostream& oss,
	std::string const& parse_string,
	std::string const& delimiter,
	std::vector<liblas::FilterPtr>& filters,
	std::vector<liblas::TransformPtr>& transforms,
	boost::array<boost::uint32_t, 4> precisions,
	bool bPrintLabels,
	bool bPrintHeader,
	bool verbose)
{

	liblas::Summary summary;

	reader.SetFilters(filters);
	reader.SetTransforms(transforms);


	if (verbose)
		std::cout << "Writing points:"
		<< "\n - : "
		<< std::endl;

	//
	// Translation of points cloud to features set
	//
	boost::uint32_t i = 0;
	boost::uint32_t const size = reader.GetHeader().GetPointRecordsCount();

	if (bPrintHeader)
	{
		oss << GetHeader(reader);
	}

	if (bPrintLabels)
	{
		oss << GetLabels(parse_string, delimiter);
	}


	while (reader.ReadNextPoint())
	{
		liblas::Point const& p = reader.GetPoint();
		// summary.AddPoint(p);
		std::string output = GetPointString(parse_string, delimiter, p, precisions, i);

		oss << output;
		if (verbose)
			term_progress(std::cout, (i + 1) / static_cast<double>(size));
		i++;

	}
	if (verbose)
		std::cout << std::endl;


}


