#include <iostream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/algorithm/string.hpp>
#include "format.h"
#include "helpers.hpp"
#include "lsb.hpp"
#include "lsb_alt.hpp"
#include "dct.hpp"
#include "dwt.hpp"
#include "tlv.hpp"
#if _WIN32
	#include <conio.h>
#else
	#define _getche getchar
#endif

using namespace cv;
using namespace std;
using namespace boost;

/*!
 * Evaluates the similarity and prints the original and resulting strings.
 *
 * \param input Original input.
 * \param output Extracted output.
 */
void print_debug(const string& input, const string& output)
{
	auto original  = clean(input);
	auto extracted = clean(output);
	auto accuracy  = similarity(original, extracted);

	Format::ColorCode color;

	if (accuracy > 99.99)
	{
		color = Format::Green;
	}
	else if (accuracy > 75)
	{
		color = Format::Yellow;
	}
	else
	{
		color = Format::Red;
	}

	cout << endl
		 << "  Similarity: " << setprecision(3) << color << Format::Bold << accuracy << "%" << Format::Normal << Format::Default << endl << endl
		 << "  Input:"       << endl << endl << Format::White << Format::Bold << original  << Format::Normal << Format::Default << endl << endl
		 << "  Extracted:"   << endl << endl << Format::White << Format::Bold << extracted << Format::Normal << Format::Default << endl;
}

/*!
 * Displays the original image and pre-steganography histogram.
 */
void show_image(const Mat& img, const string& modifier = "", bool histogram = true)
{
	static auto i = 0;

	i++;

	auto title = modifier.empty() ? "Image " + to_string(i) : modifier + " Image";

	namedWindow(title, NULL);
	resizeWindow(title, 512, 512);
	moveWindow(title, 50 + (515 * (i - 1)), 50);
	imshow(title, img);

	if (histogram)
	{
		title = modifier.empty() ? "Histogram " + to_string(i) : modifier + " Histogram";

		showHistogram(img, title.c_str());
		moveWindow(title, 50 + (515 * (i - 1)), 595);
	}
}

/*!
 * Tests the discrete cosine transformation method.
 */
void test_dct()
{
	auto img = imread("test/lena.jpg");

	show_image(img, "Original");

	auto input  = read_file("test/test.txt");
	auto stego  = encode_dct(img, input);
	auto output = decode_dct(stego);

	print_debug(input, output);

	show_image(stego, "Altered");
}

/*!
 * Tests the discrete cosine transformation method with 80% JPEG compression
 * and multi-channel message reconstruction.
 */
void test_dct_multi()
{
	auto img = imread("test/lena.jpg");

	show_image(img, "Original");

	auto input = read_file("test/test.txt");
	auto stego = encode_dct(img,   input, STORE_FULL, 0);
	     stego = encode_dct(stego, input, STORE_FULL, 1);
		 stego = encode_dct(stego, input, STORE_FULL, 2);

	imwrite("test/lena_dct.jpg", stego, vector<int> { CV_IMWRITE_JPEG_QUALITY, 80 });
	stego = imread("test/lena_dct.jpg");

	auto output = repair(vector<string>
		{
			decode_dct(stego, 0),
			decode_dct(stego, 1),
			decode_dct(stego, 2)
		});

	print_debug(input, output);

	show_image(stego, "Altered");
}
/*!
 * Prompts the user for a selection from the available options.
 *
 * \param opts Available options.
 *
 * \return Option selected by user.
 */
char get_selection(const string& opts)
{
	cout << "  Selection: " << Format::Green << Format::Bold;

	char sel;
	while ((sel = _getche()))
	{
		if (opts.find(sel) != string::npos)
		{
			break;
		}

		cout << Format::Normal << Format::Default << endl
			 << "  Selection: " << Format::Green << Format::Bold;
	}

	cout << Format::Normal << Format::Default << endl;

	return sel;
}

/*!
 * Translates a STORE_* constant into a string.
 *
 * \param store Constant value to translate.
 *
 * \return Translated value.
 */
string store_to_string(int store)
{
	switch (store)
	{
	case STORE_ONCE:   return "Store Once, Leave Rest Random";
	case STORE_FULL:   return "Store Once, Fill Rest w/ Zeros";
	case STORE_REPEAT: return "Store as Indefinitely Repeating";
	default:           return "Unknown Mode " + to_string(store);
	}
}

/*!
 * Prompts the user to select a storage mode.
 *
 * \param store Storage variable to manipulate.
 */
void select_store(int& store)
{
	switch (show_menu("Storage Mode", {
		{ 'o', "Store Once, Leave Rest Random" },
		{ 'z', "Store Once, Fill Rest w/ Zeros" },
		{ 'r', "Store as Indefinitely Repeating" },
		{ 'b', "Back to Main Menu" }
	}))
	{
	case 'o': store = STORE_ONCE;   break;
	case 'z': store = STORE_FULL;   break;
	case 'r': store = STORE_REPEAT; break;
	}
}

/*!
 * Translates the channel parameter value into a string.
 *
 * \param channel Parameter value to translate.
 *
 * \return Translated value.
 */
string channel_to_string(int channel)
{
	switch (channel)
	{
	case 0:  return "Encode All Channels";
	case 1:  return "Encode Blue Channel";
	case 2:  return "Encode Green Channel";
	case 3:  return "Encode Red Channel";
	default: return "Unknown Mode " + to_string(channel);
	}
}

/*!
 * Prompts the user to select a channel.
 *
 * \param channel Channel variable to manipulate.
 */
void select_channel(int& channel)
{
	switch (show_menu("Storage Mode", {
		{ 'a', "Encode All Channels" },
		{ 'k', "Encode Blue Channel" },
		{ 'g', "Encode Green Channel" },
		{ 'r', "Encode Red Channel" },
		{ 'b', "Back to Main Menu" }
	}))
	{
	case 'a': channel = 0; break;
	case 'k': channel = 1; break;
	case 'g': channel = 2; break;
	case 'r': channel = 3; break;
	}
}

/*!
 * Prompts the user to provide a string value.
 *
 * \param title Name of the variable.
 * \param value Variable to manipulate.
 * \param checkFile Check if value is an existing file.
 */
void prompt_string(const string& title, string& value, bool checkFile = false)
{
	cout << endl << "  " << title << ": " << Format::Green << Format::Bold;

	string str;
	getline(cin, str);
	trim(str);

	cout << Format::Normal << Format::Default;

	if (str.length() > 0)
	{
		if (checkFile)
		{
			ifstream fs(str);
			if (fs.good())
			{
				value = str;
			}
			else
			{
				cerr << "    " << Format::Red << Format::Bold << "Error:" << Format::Normal << Format::Default << " Specified file '" << str << "' does not exist." << endl;
			}
		}
		else
		{
			value = str;
		}
	}
}

/*!
 * Prompts the user to provide an integer value.
 *
 * \param title Name of the variable.
 * \param value Variable to manipulate.
 * \param min Lower limit of the value.
 * \param max Upper limit of the value.
 */
void prompt_int(const string& title, int& value, int min = INT_MIN, int max = INT_MAX)
{
	cout << endl << "  " << title << (min != INT_MIN && max != INT_MAX ? " [" + to_string(min) + "-" + to_string(max) + "]" : "") << ": " << Format::Green << Format::Bold;

	string str;
	getline(cin, str);
	trim(str);

	cout << Format::Normal << Format::Default;

	if (str.length() > 0)
	{
		auto num = atoi(str.c_str());

		if (num <= max && num >= min)
		{
			value = num;
		}
		else
		{
			cerr << "    " << Format::Red << Format::Bold << "Error:" << Format::Normal << Format::Default << " Specified value " << num << " out of range " << to_string(min) << " - " << to_string(max) << "." << endl;
		}
	}
}

/*!
 * Prompts the user to provide a double value.
 *
 * \param title Name of the variable.
 * \param value Variable to manipulate.
 * \param min Lower limit of the value.
 * \param max Upper limit of the value.
 */
void prompt_double(const string& title, double& value, double min = DBL_MIN, double max = DBL_MAX)
{
	cout << endl << "  " << title << (min != DBL_MIN && max != DBL_MAX ? " [" + to_string(min) + "-" + to_string(max) + "]" : "") << ": " << Format::Green << Format::Bold;

	string str;
	getline(cin, str);
	trim(str);

	cout << Format::Normal << Format::Default;

	if (str.length() > 0)
	{
		auto num = atof(str.c_str());

		if (num <= max && num >= min)
		{
			value = num;
		}
		else
		{
			cerr << "    " << Format::Red << Format::Bold << "Error:" << Format::Normal << Format::Default << " Specified value " << num << " out of range " << to_string(min) << " - " << to_string(max) << "." << endl;
		}
	}
}

/*!
 * Runs the discrete cosine transformation method.
 *
 * \param input Path to original image.
 * \param secret Path to the data to be hidden.
 * \param store Storage mode.
 * \param channel Channels to encode.
 * \param persistence Persistence value.
 * \param compression JPEG compression percentage.
 */
void do_dct(const string& input, const string& secret, int store, int channel, int persistence, int compression)
{
	auto img = imread(input);

	if (!img.data)
	{
		cerr << endl << "  " << Format::Red << Format::Bold << "Error:" << Format::Normal << Format::Default << " Failed to open input image from '" << input << "'." << endl << endl;
		return;
	}

	show_image(img, "Original");

	auto data = read_file(secret);

	Mat stego;

	if (channel == 0)
	{
		stego = encode_dct(img,   data, store, 0, persistence);
		stego = encode_dct(stego, data, store, 1, persistence);
		stego = encode_dct(stego, data, store, 2, persistence);
	}
	else
	{
		stego = encode_dct(img, data, store, channel - 1, persistence);
	}

	auto altered = remove_extension(input) + ".dct.jpg";

	imwrite(altered, stego, vector<int> { CV_IMWRITE_JPEG_QUALITY, compression });

	cout << endl << "  " << Format::Green << Format::Bold << "Success:" << Format::Normal << Format::Default << " Altered image written to '" << altered << "'." << endl;

	stego = imread(altered);

	string output;

	if (channel == 0)
	{
		output = repair(vector<string>
			{
				decode_dct(stego, 0),
				decode_dct(stego, 1),
				decode_dct(stego, 2)
			});
	}
	else
	{
		output = decode_dct(stego, channel - 1);
	}

	print_debug(data, output);

	show_image(stego, "Altered");
}

/*!
 * Runs the discrete cosine transformation extraction method.
 *
 * \param altered Path to the altered image.
 * \param channel Channels to decode.
 */
void read_dct(const string& altered, int channel)
{
	auto stego = imread(altered);

	if (!stego.data)
	{
		cerr << endl << "  " << Format::Red << Format::Bold << "Error:" << Format::Normal << Format::Default << " Failed to open altered image from '" << altered << "'." << endl << endl;
		return;
	}

	string output;

	if (channel == 0)
	{
		output = repair(vector<string>
			{
				decode_dct(stego, 0),
				decode_dct(stego, 1),
				decode_dct(stego, 2)
			});
	}
	else
	{
		output = decode_dct(stego, channel - 1);
	}

	output = clean(output);

	cout << endl << "  Extracted:" << endl << endl << Format::White << Format::Bold << output << Format::Normal << Format::Default << endl << endl;
}


/*!
 * Entry point of the application.
 *
 * \param Number of arguments.
 * \param Argument array pointer.
 *
 * \return Value indicating exit status.
 */
int main(int argc, char** argv)
{
mndct:
			switch (show_menu("DCT Configuration", {
				{ 'i', "Input File:    " + input },
				{ 'd', "Data File:     " + secret },
				{ 's', "Storage Mode:  " + store_to_string(store) },
				{ 'c', "Channel Usage: " + channel_to_string(channel) },
				{ 'p', "Persistence:   " + to_string(persistence) + "%" },
				{ 'j', "Compression:   " + to_string(compression) + "%" },
				{ 'a', "Perform Steganography" },
				{ 'x', "Perform Extraction" },
				{ 'b', "Back to Main Menu" }
			}, "ax"))
			{
			case 'i':
				prompt_string("Input File", input, true);
				goto mndct;

			case 'd':
				prompt_string("Data File", secret, true);
				goto mndct;

			case 's':
				select_store(store);
				goto mndct;

			case 'c':
				select_channel(channel);
				goto mndct;

			case 'p':
				prompt_int("Persistence Percentage", persistence, 0, 100);
				goto mndct;

			case 'j':
				prompt_int("JPEG Compression Percentage", compression, 0, 100);
				goto mndct;

			case 'a':
				do_dct(input, secret, store, channel, persistence, compression);
				cvWaitKey();
				break;

			case 'x':
				read_dct(input, channel);
				system("pause");
				break;

			case 'b':
				goto main;
			}
		}
		break;

	case 'x':
		return EXIT_SUCCESS;
	}
	
	return EXIT_SUCCESS;
}
