#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const char *window_name = "Klusterize (c) Jay Bosamiya";
Mat input_image, output_image;

int cluster_count = 16;
int distance_weighting = 0;

void run_k_means(int ignore_1 = 0, void * ignore_2 = NULL) {
  output_image = input_image.clone();

  imshow(window_name, output_image);
}

void show_help(const char *progname) {
    cerr <<"Usage: " << progname << " input_image output_image" << endl;
}

int main( int argc, char** argv ) {
  if( argc < 3 ) {
    show_help(argv[0]);
    return -1;
  }

  input_image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  if ( ! input_image.data ) {
      show_help(argv[0]);
      cerr <<  "ERROR: Could not open or find the image " << argv[1] << endl;
      return -1;
  }

  namedWindow(window_name, CV_WINDOW_AUTOSIZE);
  createTrackbar("Number of clusters: ", window_name, &cluster_count, 256, run_k_means);
  createTrackbar("Distance Weighting: ", window_name, &distance_weighting, 100, run_k_means);

  run_k_means();

  bool saved_once_atleast = false;
  while (true) {
    char input = waitKey(0);
    if ( input == 's' ) {
      imwrite(argv[2],output_image);
      saved_once_atleast = true;
    } else if ( input == 'q' ) {
      break;
    }
  }

  if ( ! saved_once_atleast ) {
    imwrite(argv[2],output_image);
  }

  return 0;
}
