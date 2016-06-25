function [ Y_dc, Yscore_dc, C_dc, myC_dc ] = TestDC( dc, Xtest, Ytest, th )
%TESTDC test a descriminant classifier using test data set
%   input 'dc' is a trained discriminant classifier and 'Xtest' is the data
%   set to be tested and 'Ytest' is the classified data used to measure the
%   performance. 'th' is the value around which the result of this
%   test will be rounded and it can be from 0.0 to 1.0

% Make a prediction on test data
[Y_dc, Yscore_dc] = dc.predict(Xtest);

% round Y_dc values based on threshold and Yscore_dc float values
Y_dc = (Yscore_dc(:,2) >= th) * 1;

% Compute the confusion matrix in percent (%)
% Confusion matrix is 2-by-2 readed as follows:
%{
Predicted NO        Actual NO - Predicted NO (OR) Predicted YES -Actual YES
Predicted YES - Actual YES - (1,2)       Actual YES
%}
C_dc = confusionmat(Ytest,Y_dc);

%{
my confusion matrix:
            
            Predicted NO    Predicted YES
            Actual NO       Actual YES
%}
actual_no = sum(Ytest(:)==0);
actual_yes = sum(Ytest(:)==1);
predicted_no = sum(Y_dc(:)==0);
predicted_yes = sum(Y_dc(:)==1);

myC_dc(1,1) = predicted_no;
myC_dc(1,2) = predicted_yes;
myC_dc(2,1) = actual_no;
myC_dc(2,2) = actual_yes;
%{
difference = 0;
[rows, cols] = size(Ytest);
for row=1:rows
    for col=1:cols
        if Ytest(row,col) ~= Y_dc(row,col)
            difference = difference + 1;
        end
    end
end
%}
end