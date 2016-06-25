function dc = TrainDC( Xtrain, Ytrain )
% train a discriminant classifier
%   input 'Xtrain' and supervised output 'Ytrain'.
%   output is a discriminant classifier which can be used on test sets

% train discriminant classifier
dc = ClassificationDiscriminant.fit(Xtrain,Ytrain);
end

