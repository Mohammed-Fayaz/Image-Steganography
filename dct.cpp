#include <bits/stdc++.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/*!
 * Stores the specified input once.
 */
#define STORE_ONCE   1

/*!
 * Stores the specified input and fills the rest of the available space with zeros.
 */
#define STORE_FULL   2

/*!
 * Stores the specified input in a repeating manner.
 */
#define STORE_REPEAT 3

/*!
 * Uses discrete cosine transformation to hide data in the coefficients of a channel of an image.
 *
 * \param img Input image.
 * \param text Text to hide.
 * \param mode Storage mode, see STORE_* constants.
 * \param channel Channel to manipulate.
 * \param intensity Persistence of the hidden data.
 *
 * \return Altered image with hidden data.
 */

using namespace std;
using namespace cv;

const string SENTINEL = "6969";

inline cv::Mat encode_dct(const cv::Mat& img, std::string text, int mode = STORE_ONCE, int channel = 0, int intensity = 30)
{
	using namespace cv;
	using namespace std;

	text.append(SENTINEL);

	auto block_width  = 8;
	auto block_height = 8;
	auto grid_width   = img.cols / block_width;
	auto grid_height  = img.rows / block_height;

	auto i = 0;
	auto size = text.length() * 8;

	Mat imgfp;
	img.convertTo(imgfp, CV_32F);

	vector<Mat> planes;
	split(imgfp, planes);

	for (int x = 1; x < grid_width; x++)
	{
		for (int y = 1; y < grid_height; y++)
		{
			auto px = (x - 1) * block_width;
			auto py = (y - 1) * block_height;

			Mat block(planes[channel], Rect(px, py, block_width, block_height));
			Mat trans(Size(block_width, block_height), block.type());

			dct(block, trans);

			auto a = trans.at<float>(6, 7);
			auto b = trans.at<float>(5, 1);
			
			if (i >= size)
			{
				if (mode == STORE_ONCE)
				{
					break;
				}
				else if (mode == STORE_REPEAT)
				{
					i = 0;
				}
			}

			auto val = 0;
			if (i < size)
			{
				val = (text[i / 8] & 1 << i % 8) >> i % 8;
				i++;
			}

			if (val == 0)
			{
				if (a > b)
				{
					swap(a, b);
				}
			}
			else
			{
				if (a < b)
				{
					swap(a, b);
				}
			}

			if (a > b)
			{
				auto d = (intensity - (a - b)) / 2;
				     a = a + d;
				     b = b - d;
			}
			else
			{
				auto d = (intensity - (b - a)) / 2;
				     a = a - d;
				     b = b + d;
			}

			trans.at<float>(6, 7) = a;
			trans.at<float>(5, 1) = b;

			Mat stego(Size(block_width, block_height), block.type());

			idct(trans, stego);

			stego.copyTo(planes[channel](Rect(px, py, block_width, block_height)));
		}

		if (i >= size && mode == STORE_ONCE)
		{
			break;
		}
	}

	Mat mergedfp;
	merge(planes, mergedfp);

	Mat merged;
	mergedfp.convertTo(merged, CV_8U);

	return merged;
}

/*!
 * Uses discrete cosine transformation to recover data hidden in the coefficients of an image.
 *
 * \param img Input image with hidden data.
 * \param channel Channel to manipulate.
 *
 * \return Hidden data extracted form image.
 */
inline std::string decode_dct(const cv::Mat& img, int channel = 0)
{
	using namespace cv;
	using namespace std;

	auto block_width  = 8;
	auto block_height = 8;
	auto grid_width   = img.cols / block_width;
	auto grid_height  = img.rows / block_height;

	auto i = 0;
	string bits(grid_width * grid_height / 8, 0);

	Mat imgfp;
	img.convertTo(imgfp, CV_32F);

	vector<Mat> planes;
	split(imgfp, planes);

	for (int x = 1; x < grid_width; x++)
	{
		for (int y = 1; y < grid_height; y++)
		{
			auto px = (x - 1) * block_width;
			auto py = (y - 1) * block_height;

			Mat block(planes[channel], Rect(px, py, block_width, block_height));
			Mat trans(Size(block_width, block_height), block.type());

			dct(block, trans);

			auto a = trans.at<float>(6, 7);
			auto b = trans.at<float>(5, 1);

			if (a > b)
			{
				bits[i / 8] |= 1 << i % 8;
			}

			i++;
		}
	}

	return bits;
}

/*!
 * Reads the specified file into a string.
 *
 * \param file Path to the file.
 *
 * \return Contents of the file.
 */
inline std::string read_file(const std::string& file)
{
	std::ifstream fs(file);
	std::string text((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
	fs.close();
	return text;
}

int main(int argc, char *argv[]){
	if (string(argv[1]) == "encode"){
		auto img = imread(argv[2]);
		auto input  = read_file(argv[3]);
		auto stego  = encode_dct(img, input);
		auto output = decode_dct(stego);
		size_t found = output.find(SENTINEL);
		auto normalized_output = output.substr(0, found);
		cout << "The secret message is: " << normalized_output << endl;
		imwrite(argv[4], stego, vector<int> {CV_IMWRITE_JPEG_QUALITY, 80});
	}
	else if (string(argv[1]) == "decode"){
		auto output = decode_dct(imread(argv[2]));
		size_t found = output.find(SENTINEL);
		auto normalized_output = output.substr(0, found);
		cout << "The secret message is: " << normalized_output << endl;
		imshow("Image", imread(argv[2]));
		waitKey(0);
	}
	else{
		cout << "Undefined Option" << endl;
		return 0;
	}

	return 0;
}