function rout = salinity_stats(filename)
%
% ME 121-001
% HW 3: Group Assignment
% Sean Lai
% 2/3/20
%
% salinty_stats  Load raw salinity data and plot histogram
%
% Input:  filename = (string) optional name of plain text file
%                    that contains the data.  Default:  'salinity10a.txt'
% 
% Generates a histogram for each column in dataset
% Prints statistics for each column in data set
%
% Output: None

% -- Set filename
if nargin<1, filename='calibration_data.csv';  end

% -- Load data from a csv file of the four data sets
r = importdata(filename);

% -- Function to create a histogram
function create_histogram(data)
    figure
    histogram(data)
    xlabel('raw reading')
    ylabel('number of readings')
end

% -- Create a histogram for each column in r
for i = 1:4
    create_histogram(r(:,i))
end

% -- Function to print out summary of statistics for a data set
function print_stats(data, column_number)
    fprintf('\tColumn number:       %d\n', column_number);
    fprintf('\tNumber of values:    %d\n',length(data));
    fprintf('\tMean:                %5.2f\n',mean(data));
    fprintf('\tMedian:              %5.1f\n',median(data));
    fprintf('\tStandard deviation:  %5.3f\n',std(data));
end

% -- Print statistics for each data set to command window
fprintf('\nData in %s:\n',filename)
for i = 1:4
    print_stats(r(:,i), i)
end
