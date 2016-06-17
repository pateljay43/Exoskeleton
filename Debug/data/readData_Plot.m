function [data30,data60] = readData_Plot(prefix)
% This function reads fileName for given samples and return the data

files = textread('metaData.txt');

% row id for data30 and data60
colId = [1 1];
data30 = [];
for id=1:files
    fileName = [prefix num2str(id-1) '.csv'];
    temp = csvread(fileName);
    [rows, cols] = size(temp);
    if cols == 31
        data30(:,colId(1,1)) = temp(1,1:cols-1);
        colId = colId + [1 0];
    elseif cols == 61
        data60(:,colId(1,2)) = temp(1,1:cols-1);
        colId = colId + [0 1];
    end
end

figure(1);
set(1,'DefaultFigureWindowStyle', 'docked');
plot(data30,'DisplayName','data30');
figure(2);
set(2,'DefaultFigureWindowStyle', 'docked');
plot(data60,'DisplayName','data60');

