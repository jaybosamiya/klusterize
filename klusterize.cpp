#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const char *window_name = "Klusterize (c) Jay Bosamiya";
Mat input_image, output_image;

int cluster_count = 16;
int distance_weighting = 0;
const int tile_every = 40;

typedef Vec<float, 5> Vec5f;

Vec5f calc_location_and_colour(int r, int c, unsigned char red, unsigned char green, unsigned char blue) {
  Vec5f t;

  t[0] = (distance_weighting / 100.0) * (r / (float)input_image.rows);
  t[1] = (distance_weighting / 100.0) * (c / (float)input_image.cols);
  t[2] = (1 - (distance_weighting / 100.0)) * (red   / 255.0);
  t[3] = (1 - (distance_weighting / 100.0)) * (green / 255.0);
  t[4] = (1 - (distance_weighting / 100.0)) * (blue  / 255.0);

  return t;  
}

void run_k_means(int ignore_1 = 0, void * ignore_2 = NULL) {
  if ( cluster_count < 2 ) { cluster_count = 2; }

  vector<Vec5f> data_values;
  for ( int r = 0 ; r < input_image.rows ; r += tile_every ) {
    for ( int c = 0 ; c < input_image.cols ; c += tile_every ) {
      unsigned char red   = input_image.at<Vec3b>(r,c)[2];
      unsigned char green = input_image.at<Vec3b>(r,c)[1];
      unsigned char blue  = input_image.at<Vec3b>(r,c)[0];

      data_values.push_back(calc_location_and_colour(r, c, red, green, blue));
    }
  }

  Mat labels, centers;
  kmeans(data_values, cluster_count, labels, TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0), 3, KMEANS_PP_CENTERS, centers);

  output_image = Mat(input_image.size(), CV_8UC3);

  for ( int r = 0 ; r < output_image.rows ; r++ ) { // TODO: Speed this loop up somehow
    for ( int c = 0 ; c < output_image.cols ; c++ ) {
      unsigned char red   = input_image.at<Vec3b>(r,c)[2];
      unsigned char green = input_image.at<Vec3b>(r,c)[1];
      unsigned char blue  = input_image.at<Vec3b>(r,c)[0];

      Vec5f curr = calc_location_and_colour(r, c, red, green, blue);

      Vec5f best_center = centers.at<Vec5f>(0);
      for ( int i = 0 ; i < cluster_count ; i++ ) {
        Vec5f curr_center = centers.at<Vec5f>(i);
        if ( norm(curr - curr_center) < norm(curr - best_center) ) {
          best_center = curr_center;
        }
      }

      output_image.at<Vec3b>(r,c)[2] = (unsigned char)(best_center[2] * 255.0 / (1 - (distance_weighting / 100.0)));
      output_image.at<Vec3b>(r,c)[1] = (unsigned char)(best_center[3] * 255.0 / (1 - (distance_weighting / 100.0)));
      output_image.at<Vec3b>(r,c)[0] = (unsigned char)(best_center[4] * 255.0 / (1 - (distance_weighting / 100.0)));
    }
  }

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
