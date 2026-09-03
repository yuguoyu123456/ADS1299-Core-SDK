function result = eeg_basic_analysis(data, fs, mainsHz)
%EEG_BASIC_ANALYSIS Beginner entry point for ADS1299-Core MATLAB/Octave EEG.
%
% RESULT = EEG_BASIC_ANALYSIS(DATA, FS, MAINSHZ) is retained for compatibility
% with early versions of this repository. The actual reusable implementation
% now lives in the +ads1299eeg package.
%
% Data convention: samples x channels, host-side amplitudes in microvolts.
%
% Example with synthetic data:
%   result = eeg_basic_analysis();
%
% Example with real ADS1299 data already converted to microvolts:
%   result = eeg_basic_analysis(eeg_uV, 250, 50);
%
% For new code, prefer:
%   result = ads1299eeg.basic_pipeline(eeg_uV, 250, options);

if nargin < 1 || isempty(data)
    fs = 250;
    mainsHz = 50;
    duration = 20;
    t = (0:(fs*duration-1))'/fs;
    rand('seed',7); randn('seed',7);
    data = zeros(numel(t),8);
    for ch = 1:8
        phase = 0.17*(ch-1);
        data(:,ch) = ...
            (16+ch).*sin(2*pi*10*t+phase) + ...
            4.*sin(2*pi*6*t+phase/2) + ...
            8.*sin(2*pi*mainsHz*t) + ...
            4.*randn(size(t));
    end
end
if nargin < 2 || isempty(fs), fs = 250; end
if nargin < 3 || isempty(mainsHz), mainsHz = 50; end

if exist('OCTAVE_VERSION','builtin')
    % The reusable filters use butter/filtfilt from Octave's signal package.
    try
        pkg load signal;
    catch
        error('eeg_basic_analysis:OctaveSignalPackage', ...
            'GNU Octave requires the signal package. Install/load octave-signal.');
    end
end

options = struct();
options.mainsHz = mainsHz;
options.lowHz = 0.5;
options.highHz = min(45,fs/2-0.5);
options.applyCAR = true;
result = ads1299eeg.basic_pipeline(data,fs,options);

fprintf('ADS1299-Core-SDK MATLAB/Octave EEG analysis\n');
fprintf('Samples: %d, channels: %d, fs: %.1f Hz\n', ...
    size(result.processed,1),size(result.processed,2),fs);
fprintf('Alpha peak (Hz): ');
fprintf('%.2f ',result.alphaPeakHz);
fprintf('\n');
if ~isempty(result.qualityAfter.badChannelIndices)
    fprintf('Quality warning channels: ');
    fprintf('%d ',result.qualityAfter.badChannelIndices);
    fprintf('\n');
end
end
