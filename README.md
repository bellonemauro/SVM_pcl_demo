

Tutorial to train a SVM classifier using the point cloud library

Dependencies: PCL-1.8 
          NB: the official version of PCL will not work, I have modified 
              the files smv_wrapper.h/cpp and svm.h/cpp ---  
              You can find my version here https://github.com/bellonemauro/pcl
CMake

./Data contains an example of dataset including two file : 
     model.dat
     test.dat
     train.dat
     
to train the classifier using the train.dat file run the following command:

./pcl_SVM_train_classify -t <location of /Data/example1/train.dat > 

es. I have my build inside the same folder hence:
./pcl_SVM_train_classify -t ../Data/example1/train.dat 


it is possible to save the generated model using the option -s

./pcl_SVM_train_classify -t <location of /Data/example1/train.dat > -s

in this case two files will be saved in the same binary folder :
    ./train_out.dat     ---> this is the data used as training 
    ./model_out.dat     ---> this is the model generated for the classifier

it is possible to try the classification using the following command:
./pcl_SVM_train_classify -c ../Data/example1/model.dat ../Data/example1/test.dat


it is also possible to train and test the classification at the same time
./pcl_SVM_train_classify -t ../Data/example1/train.dat -tc ../Data/example1/test.dat

