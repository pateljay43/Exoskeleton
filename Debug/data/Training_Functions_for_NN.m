% trainscg - Scaled conjugate gradient backpropagation with 10 hidden layers of
% neurons
net = feedforwardnet(10,'trainscg');
net = train(net,transpose(Xtrain),transpose(Ytrain));

% trainbfg - BFGS quasi-Newton backpropagation
% traincgb - Conjugate gradient backpropagation with Powell-Beale restarts
% traincfg - Conjugate gradient backpropagation with Fletcher-Reeves updates
% traincgp - Conjugate gradient backpropagation with Polak-Ribiére updates
% traingda - Gradient descent with adaptive learning rate backpropagation
% traingdm - Gradient descent with momentum backpropagation
% traingdx - Gradient descent with momentum and adaptive learning rate backpropagation
% trainlm  - Levenberg-Marquardt backpropagation
% trainoss - One-step secant backpropagation
% trainrp  - Resilient backpropagatio


