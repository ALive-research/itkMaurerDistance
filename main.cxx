// TCLAP includes
#include <tclap/ValueArg.h>
#include <tclap/ArgException.h>
#include <tclap/CmdLine.h>

//ITK includes
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkSignedMaurerDistanceMapImageFilter.h>
#include <itkRealTimeClock.h>

// STD includes
#include <cstdlib>
#include <string>

// ===========================================================================
// Entry point
// ===========================================================================
int main(int argc, char **argv)
{
  // =========================================================================
  // Command-line variables
  // =========================================================================
  std::string input;
  std::string output;

  // =========================================================================
  // Parse arguments
  // =========================================================================
  try
    {
    TCLAP::CmdLine cmd("itkMaurerDistance");

    TCLAP::ValueArg<std::string> inputArgument("i", "input", "Input file", true, "None", "string");
    TCLAP::ValueArg<std::string> outputArgument("o", "output", "Output file", true, "None", "string");

    cmd.add(inputArgument);
    cmd.add(outputArgument);

    cmd.parse(argc,argv);

    input =  inputArgument.getValue();
    output = outputArgument.getValue();

    }
  catch(TCLAP::ArgException &e)
    {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

  // =========================================================================
  // ITK definitions
  // =========================================================================
  using LabelImageType = itk::Image<unsigned short,3>;
  using LabelReaderType = itk::ImageFileReader<LabelImageType>;
  using DistanceImageType = itk::Image<float, 3>;
  using DistanceWriterType = itk::ImageFileWriter<DistanceImageType>;
  using SignedMaurerDistanceMapImageFilterType =
      itk::SignedMaurerDistanceMapImageFilter<LabelImageType, DistanceImageType>;

  // =========================================================================
  // Read the labelmap image
  // =========================================================================
  auto reader = LabelReaderType::New();
  reader->SetFileName(input);
  reader->Update();

  // =========================================================================
  // Apply the Maurer distance
  // =========================================================================
  auto realTimeClock = itk::RealTimeClock::New();
  auto maurer = SignedMaurerDistanceMapImageFilterType::New();
  maurer->SetInput(reader->GetOutput());
  auto start = realTimeClock->GetRealTimeStamp();
  maurer->Update();
  auto end = realTimeClock->GetRealTimeStamp();

  std::cout << "Time (ms):" << (end - start).GetTimeInMilliSeconds() << std::endl;

  // =========================================================================
  // Output writing
  // =========================================================================
  auto writer = DistanceWriterType::New();
  writer->SetFileName(output);
  writer->SetInput(maurer->GetOutput());

  try
  {
    writer->Update();
  }
  catch (const itk::ExceptionObject & excp)
  {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
