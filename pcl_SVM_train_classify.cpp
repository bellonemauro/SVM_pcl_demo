/*  +---------------------------------------------------------------------------+
 *  |                                                                           |
 *  |                     https://www.maurobellone.com                          |
 *  |                                                                           |
 *  | Copyright (c) 2017, - All rights reserved.                                |
 *  | Authors: Mauro Bellone                                                    |
 *  | Released under ___ license                                                |
 *  +---------------------------------------------------------------------------+ */

#include <pcl/console/parse.h>
#include <pcl/ml/svm_wrapper.h>

// --------------
// -----Help-----
// --------------
void 
printUsage (const char* progName)
{
  std::cout << "\n\n" 
            << " +---------------------------------------------------------------------------+ \n"
            << " |                     pcl SVM training example help                         | \n"
            << " +---------------------------------------------------------------------------+ \n"
            << " |   \t  Description:  Example of the SVM usage within the                  | \n"
            << " |                      point cloud libray                                   | \n"
            << " |                                                                           | \n"
            << " +---------------------------------------------------------------------------+ \n"
            << " |   \t  Usage: "<<progName<<" [options] <file1.dat>  <file2.dat> | \n"
            << " |                                                                           | \n"
            << " +---------------------------------------------------------------------------+ \n"
            << " |   \t  Options:                                                           | \n"
            << " |                                                                           | \n"
            << " |       -h / --help       --->   visualize this help                        | \n"
            << " |       -c / --classify   --->   classify new data                          | \n"
            << " |                                using the model in <file1.dat>             | \n"
            << " |                                and the data in <file2.dat>                | \n"
            << " |       -t / --train      --->   train the classifier using <file.dat>      | \n"
            << " |       -tc               --->   train the classifier using <file1.dat>     | \n"
            << " |                                and try the classifier using <file2.dat>   | \n"
            << " |                         NOTE : -tc has to be used with -t                 | \n"
            << " |       -s / --save       --->   save training results and model:           | \n"
            << " |                                  training data to <./train_out.dat>       | \n"
            << " |                                  model to <./model_out.dat>               | \n"
            << " |                                                                           | \n"
            << " +---------------------------------------------------------------------------+ \n" << std::endl;
            
}


int main (int argc, char** argv)
{
  // define what we need to configure the SVM classifier/trainer
  pcl::SVMTrain my_svm_trainer; //--> our trainer, to be used for store training data or for a new training procedure
  pcl::SVMClassify my_svm_classifier;  //--> our classifier
  std::vector<pcl::SVMData> my_training_set;   //--> the training set is a vector of data
  pcl::SVMModel my_svm_model;   //--> classifier model, this is automatically generated after the training or loaded for the classification
  pcl::SVMParam my_svm_parameters; //--> our own configuration parameters

  std::vector<int> filename; 
  std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
  std::cout.precision(3);
  std::cout << "\n\n" 
            << " +---------------------------------------------------------------------------+ \n"
            << " |                       pcl SVM training example                            | \n"
            << " +---------------------------------------------------------------------------+ \n";

  // --------------------------------------
  // -----Parse Command Line Arguments-----
  // --------------------------------------
  if (pcl::console::find_argument (argc, argv, "-h") >= 0 || 
        pcl::console::find_argument (argc, argv, "--help") >= 0)   // the help
  {
    printUsage (argv[0]);
    return 0;
  }
  if ( pcl::console::find_argument (argc, argv, "-c") >= 0 || 
       pcl::console::find_argument (argc, argv, "--classify") >= 0)  // the classifier
  {
    filename = pcl::console::parse_file_extension_argument (argc, argv, "dat");
    if (!filename.empty ())
    {
      std::string model_filename = argv[filename[0]];  //the first file is the model, whereas the second contains the test data
      if ( my_svm_classifier.loadClassifierModel( model_filename.c_str() ) ) 
      { 
        std::cout << "\t The model has been successfully loaded \n";
        std::cout << "\t Parsed test file " << model_filename << "\n\n";
      }
      else
      {
        std::cout << "<<pcl_SVM_training example error >> : Was not able to open file \""<<model_filename<<"\".\n";
        //printUsage (argv[0]);  //the error is already prented by the function loadClassifierModel()
        return 0;  //exit if we cannot open the classification model
      }
      if (filename.size()>1)
      {
        std::string testData_filename = argv[filename[1]];  //the first file is the model, whereas the second contains the test data
        if ( my_svm_classifier.loadClassProblem(testData_filename.c_str()))
        {
           std::cout << "\t The problem data have been successfully loaded \n";
           std::cout << "\t Parsed test file " << testData_filename << "\n\n";
        }
        else  
        {
           std::cout << "<<pcl_SVM_training example error >> : Was not able to open file \""<<testData_filename<<"\".\n";
           //printUsage (argv[0]);
           return 0;
        }
      }
      else  
      {
        std::cout << "\t No data file for classification, abort ! \n\n";
        return 0;
      }

      my_svm_parameters = my_svm_classifier.getParameters();

      // test the classifier 
      std::vector< std::vector<double> > classification_result;
      my_svm_classifier.setProbabilityEstimates((my_svm_parameters.probability?false:true));

      if ( my_svm_classifier.classification( ) )
        std::cout << "\t Classification DONE ! \n";
      else {
        std::cout << "\t Classification ERROR --- Exit now ! \n\n";
        return 0;
      }

      if ( my_svm_classifier.hasLabelledTrainingSet())
      {
         std::cout << "\t Loaded dataset has labels, the classification test will run \n";
         if ( my_svm_classifier.classificationTest( ) ) 
         {
           my_svm_classifier.getClassificationResult(classification_result);
           std::cout << "\t  Classification result size = \t  " << classification_result.size() << " \n";
           std::cout << "\t Classification test SUCCESS ! \n\n";
         }
         else  
         {
           std::cout << "\t Classification test NOT SUCCESS \n\n";  
           return 0;
         }
      }
      else 
      {
        std::cout << "\t Loaded dataset has NO labels, the classification test cannot be executed \n"; 
        return 0;
      }

      // run the classification and return the number of positive/negative samples
      int number_of_positive_samples = 0;
      int number_of_negative_samples = 0;
      int number_of_unclassified_samples = 0;
      for (size_t i = 0; i < classification_result.size(); i++) {
       for (size_t j = 0; j < classification_result.at(i).size(); j++) {
          if ( classification_result.at(i).at(j) == 1 ) {
              number_of_positive_samples++;
            }
          else {
            if ( classification_result.at(i).at(j) == -1) {
               number_of_negative_samples++; }
            else {
               number_of_unclassified_samples++;
             } 
            }
       }
      }
      std::cout << "\n\t Classification Results : \n";
      std::cout << "\t\t  number of positive samples = \t " << number_of_positive_samples << " \n";
      std::cout << "\t\t  number of negative samples = \t " << number_of_negative_samples << " \n";
      std::cout << "\t\t  number of unclassified samples = \t " << number_of_unclassified_samples << " \n";
      std::cout << "\t NOTE: using probability parameter will always results in \n"
                << "\t       high number of unclassified samples \n\n";

      pcl::SVMtestReport svm_test_report = my_svm_classifier.getClassificationTestReport();
      pcl::console::print_info ( " - Accuracy (classification) = " );
      pcl::console::print_value ( "%g%% (%d/%d)\n", 
                                   svm_test_report.accuracy, 
                                   svm_test_report.correctPredictionsIdx, 
                                   svm_test_report.totalSamples );
   }
   else 
   {
     std::cout << "\t ATTENTION : No model file given, please see the help for usage  \n\n";
     printUsage (argv[0]);
     return 0;
   }
   return 0;
   }
   if ( pcl::console::find_argument (argc, argv, "-t") >= 0 || 
        pcl::console::find_argument (argc, argv, "--train") >= 0)  // the trainer
   {
     filename = pcl::console::parse_file_extension_argument (argc, argv, "dat");
     if (!filename.empty ())
     {
       std::string trainData_filename = argv[filename[0]]; //the first file is the training data, whereas the second contains the test data
       if ( my_svm_trainer.loadProblem(trainData_filename.c_str()) )  // load the train data file 
       {
         std::cout << "\t The training data have been successfully loaded  " << std::endl;
         std::cout << "\t Parsed test file " << trainData_filename << "\n\n";
       }
       else
       {
         std::cout << "<<pcl_SVM_training example error >> : Was not able to open file \""<<trainData_filename<<"\".\n";
         //printUsage (argv[0]);
         return 0;
       } 

       // check the training set loaded
       my_svm_trainer.adaptProbToInput();
       my_training_set = my_svm_trainer.getInputTrainingSet();
       std::cout << "\t Training set size = \t " << my_training_set.size() << " \n";

       // configure some useful parameters
       // TODO: let the user configure this parameters from the command line !
       my_svm_parameters.kernel_type = RBF;
       my_svm_parameters.shrinking = 1;
       my_svm_parameters.gamma = 0.0005;
       my_svm_parameters.C = 10;
       my_svm_parameters.probability = 0;
       my_svm_trainer.setParameters(my_svm_parameters);  // set the parameters for the trainer

       // train the classifier
       if (my_svm_trainer.trainClassifier() )
       { 
         std::cout << "\t The classifier has been successfully tranined \n\n";
       }
       else
       { 
         std::cout << "\t The classifier has NOT been tranined  - Exit now ! \n\n";
         return 0;
       }

       // check the model for the classifier
       my_svm_model = my_svm_trainer.getClassifierModel();    

       std::cout << "\t Model parameters summary : \n";
       if ((my_svm_parameters.probability?true:false)){
          std::cout << "\t\t  Probability support \t active  \n";
          std::cout << "\t\t  ProbA = \t" << *my_svm_model.probA << " \n"
                    << "\t\t  ProbB = \t" << *my_svm_model.probB << " \n";
       }
       else {
         std::cout << "\t\t  Probability support \t NOT active  \n";
       }
       std::cout << "\t\t  l  \t \t \t " << my_svm_model.l  << " \n";
       std::cout << "\t\t  Number of classes   \t " << my_svm_model.nr_class << " \n";
       std::cout << "\t\t  sv_coef   \t  \t " <<  *(*my_svm_model.sv_coef) << " \n";
       std::cout << "\t\t  Rho   \t  \t " <<  *my_svm_model.rho << " \n";
       std::cout << "\t\t  label   \t  \t " <<  *my_svm_model.label << " \n";
       std::cout << "\t\t  nSV \t   \t  \t " <<  *my_svm_model.nSV << " \n\n";

       // save the training results, i.e. the training set and the generated model
       if ( pcl::console::find_argument (argc, argv, "-s") >= 0 || 
            pcl::console::find_argument (argc, argv, "--save") >= 0)
       {
         if ( my_svm_trainer.saveTrainingSet("./train_out.dat") )
         { 
            std::cout << "\t Training results saved in ./train_out.dat \n";
         }
         else {
            std::cout << "\t training results not saved. Exit now.  \n\n";
            return 0;
         }

         if ( my_svm_trainer.saveClassifierModel("./model_out.dat") ) {
            std::cout << "\t Model results saved in ./model_out.dat \n";
         }
         else  {
            std::cout << " |   \t Model results not saved. Exit now. \n";
            return 0;
         }
      }

    bool exportData = false;
    if (exportData){// try to export some of the training data from the std::vector<dataset>
        pcl::SVMData my_svm_dataA;  //--> dataset A
        pcl::SVMData my_svm_dataB;  //--> dataset B
        if (my_training_set.size() >1 )
          {
            my_svm_dataA = my_training_set.at(0);
            my_svm_dataB = my_training_set.at(1);
            std::cout << " +---------------------------------------------------------------------------+ \n"
                      << " |                      Point numbers in the dataset                         | \n"
                      << " +---------------------------------------------------------------------------+ \n"
                      << " |   \t Dataset A:                                                          | \n"
                      << " |   \t  Label   \t\t Feature   \t\t   value             | \n"
                      << " +---------------------------------------------------------------------------+ \n";

            for (size_t i = 0; i < my_svm_dataA.SV.size(); i++)
                {
                  std::cout << " |   \t  " << my_svm_dataA.label << " "
                            << "     \t \t  " << my_svm_dataA.SV.at(i).idx  << " "
                            << "     \t \t  " << my_svm_dataA.SV.at(i).value << "              |" << std::endl;
               }

            std::cout << " +---------------------------------------------------------------------------+ \n"
                      << " |   \t Dataset B:                                                          | \n"
                      << " |   \t  Label   \t\t Feature   \t\t   value             | \n"
                      << " +---------------------------------------------------------------------------+ \n";
            for (size_t i = 0; i < my_svm_dataB.SV.size(); i++)
                {
                  std::cout << " |   \t  " << my_svm_dataB.label << " "
                            << "     \t \t  " << my_svm_dataB.SV.at(i).idx  << " "
                            << "     \t \t  " << my_svm_dataB.SV.at(i).value << "              |" << std::endl;
               }
            std::cout << " +---------------------------------------------------------------------------+ \n";
         }
    }

    // try to make a classification test after the training
    if ( pcl::console::find_argument (argc, argv, "-tc") >= 0 )
      {
       my_svm_classifier.setClassifierModel(my_svm_model);

       if (filename.size()>1)
       {
         std::string testData_filename = argv[filename[1]];

         if ( my_svm_classifier.loadClassProblem(testData_filename.c_str()))
           {
             std::cout << "\t The problem data have been successfully loaded  " << std::endl;
             std::cout << "\t Parsed test file " << testData_filename << "\n\n";
         }
         else  {
             //std::cout << "<<pcl_SVM_training example error >> : Was not able to open file \""<<trainData_filename<<"\".\n";
             //printUsage (argv[0]);
             return 0;
           }
       }
       else  {
         std::cout << "\t No data file for classification, abort ! \n";
         return 0;
       }

       my_svm_classifier.setProbabilityEstimates((my_svm_parameters.probability?true:false));

       if ( my_svm_classifier.classification( ) )
         std::cout << "\t Classification DONE ! \n";
       else {
         std::cout << "\t Classification ERROR --- Exit now ! \n\n";
         return 0;
       }

       // set some vars for the test report
       int number_of_positive_samples = 0;
       int number_of_negative_samples = 0;
       int number_of_unclassified_samples = 0;
       std::vector< std::vector<double> > classification_result;

       if ( my_svm_classifier.hasLabelledTrainingSet())
       {
           std::cout << "\t Loaded dataset has labels, the classification test will run \n";
       if ( my_svm_classifier.classificationTest( ) ) {
         my_svm_classifier.getClassificationResult(classification_result);
         std::cout << "\t Classification result size = \t  " << classification_result.size() << " \n";
         std::cout << "\t Classification test SUCCESS ! \n\n";
       }
       else  {
         std::cout << "\t Classification test NOT SUCCESS \n\n";  }
       }
       else std::cout << "\t Loaded dataset has NO labels, the classification test cannot be executed \n";

       my_svm_classifier.getClassificationResult(classification_result);
       std::cout << "\t  Classification result size = \t  " << classification_result.size() << " \n";
       for (size_t i = 0; i < classification_result.size(); i++) {
        for (size_t j = 0; j < classification_result.at(i).size(); j++) {
           if ( classification_result.at(i).at(j) == 1 ) {
               number_of_positive_samples++;
             }
           else {
             if ( classification_result.at(i).at(j) == -1) {
                number_of_negative_samples++; }
             else {
                number_of_unclassified_samples++;
              }
            }
         }
       }
       std::cout << "\n\t Classification Results : \n";
       std::cout << "\t\t  number of positive samples = \t " << number_of_positive_samples << " \n";
       std::cout << "\t\t  number of negative samples = \t " << number_of_negative_samples << " \n";
       std::cout << "\t\t  number of unclassified samples = \t " << number_of_unclassified_samples << " \n";
       std::cout << "\t NOTE: using probability parameter will always results in \n"
                 << "\t       high number of unclassified samples \n\n";
       
       pcl::SVMtestReport svm_test_report = my_svm_classifier.getClassificationTestReport();
       pcl::console::print_info ( " - Accuracy (classification) = " );
       pcl::console::print_value ( "%g%% (%d/%d)\n", 
                                   svm_test_report.accuracy, 
                                   svm_test_report.correctPredictionsIdx, 
                                   svm_test_report.totalSamples );
    
    }
    }
    else 
     {
       std::cout << "\t ATTENTION : No training file given, please see the help for usage  \n\n";
       printUsage (argv[0]);
       return 0;
     }
     return 0;
   }
   if (pcl::console::find_argument (argc, argv, "") <= 0)
   {
    printUsage (argv[0]);
    return 0;
   }

 return (0);
}

