%   --------------------------------------
%   Decision Tree
%   --------------------------------------
dt = fitctree(Xtrain,Ytrain);
%train with cross validation
% dt = fitctree(Xtrain,Ytrain,'CrossVal','on');

% display tree and histogram of the generated tree
numBranches = @(x)sum(x.IsBranch);
mdlDefaultNumSplits = cellfun(numBranches, dt.Trained);
figure;
histogram(mdlDefaultNumSplits);
view(dt.Trained{1},'Mode','graph');

% calculate loss/error
error = kfoldLoss(dt);


%   --------------------------------
%   Discriminant Classification
%   --------------------------------
dc = fitcdiscr(Xtrain,Ytrain,'DiscrimType','quadratic','ScoreTransform','logit');

%calculate loss
error = loss(obj,Xtest,Ytest);
%error = resubLoss(obj);

%   ---------------------------------------
%   Support Vector Machine Classifier
%   ---------------------------------------
% unresponsive
svmc = fitcsvm(Xtrain,Ytrain);


