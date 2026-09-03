% ADS1299-Core-SDK MATLAB/Octave EEG numerical regression tests.
% The script resolves the library path from its own file location so it works
% even when MATLAB/Octave RUN temporarily changes the current directory.

fprintf('ADS1299 MATLAB/Octave EEG tests\n');

% Octave requires the signal package for butter/filtfilt. MATLAB users need
% Signal Processing Toolbox for the filtering functions used by the library.
if exist('OCTAVE_VERSION','builtin')
    pkg load signal;
end

testFile = mfilename('fullpath');
testsDir = fileparts(testFile);
matlabDir = fileparts(testsDir);
addpath(matlabDir);

%% ADS1299 24-bit decoding and scaling
assert(ads1299eeg.decode24([0 0 0]) == 0);
assert(ads1299eeg.decode24([0 0 1]) == 1);
assert(ads1299eeg.decode24([127 255 255]) == 8388607);
assert(ads1299eeg.decode24([255 255 255]) == -1);
assert(ads1299eeg.decode24([128 0 0]) == -8388608);
expectedLsbUv = ((2*4.5/24)/2^24)*1e6;
actualLsbUv = ads1299eeg.codes_to_uv(1,4.5,24);
assert(abs(actualLsbUv-expectedLsbUv) < 1e-12);
fprintf('PASS ADS1299 conversion\n');

%% Synthetic 8-channel EEG
fs = 250;
duration = 20;
t = (0:(fs*duration-1))'/fs;
rand('seed',7); randn('seed',7);
data = zeros(numel(t),8);
for ch = 1:8
    phase = (ch-1)*0.19;
    data(:,ch) = ...
        (18+ch).*sin(2*pi*10*t+phase) + ...
        3.*sin(2*pi*6*t+phase/2) + ...
        8.*sin(2*pi*50*t) + ...
        2.5.*randn(size(t));
end

%% Notch attenuation
[fBefore,pBefore] = ads1299eeg.welch_psd(data,fs,1000,500);
filteredNotch = ads1299eeg.notch_filter(data,fs,50,30,1);
[fAfter,pAfter] = ads1299eeg.welch_psd(filteredNotch,fs,1000,500);
[~,lineIndexBefore] = min(abs(fBefore-50));
[~,lineIndexAfter] = min(abs(fAfter-50));
ratio = mean(pAfter(lineIndexAfter,:))/mean(pBefore(lineIndexBefore,:));
assert(ratio < 0.20);
fprintf('PASS notch attenuation %.6f\n',ratio);

%% Spectral features
peaks = ads1299eeg.alpha_peak(filteredNotch,fs,[8 13],1000);
assert(max(abs(peaks-10)) <= 0.5);
powers = ads1299eeg.bandpower(filteredNotch,fs,[],[],1000);
assert(all(powers.relative.alpha > powers.relative.theta));
assert(all(powers.relative.alpha > powers.relative.beta));
fprintf('PASS alpha peak and band power\n');

%% Quality detection
dataWithFlat = data;
dataWithFlat(:,8) = 0;
quality = ads1299eeg.quality_report(dataWithFlat,fs,50,struct('maxLineNoiseRatio',1));
assert(any(quality.badChannelIndices == 8));
reasons = quality.channels(8).reasons;
assert(any(strcmp(reasons,'flatline')) || any(strcmp(reasons,'low_variance')));
fprintf('PASS quality report\n');

%% Epoch extraction and baseline correction
events = round([2 5 8]*fs)+1;
[epochs,times,kept] = ads1299eeg.epoch_data(data,events,fs,-0.2,0.8);
assert(isequal(size(epochs),[3 250 8]));
assert(isequal(kept(:),events(:)));
corrected = ads1299eeg.baseline_correct(epochs,times,[-0.2 0]);
baselineMask = times >= -0.2 & times <= 0;
for e = 1:size(corrected,1)
    for ch = 1:size(corrected,3)
        baselineMean = mean(corrected(e,baselineMask,ch));
        assert(abs(baselineMean) < 1e-10);
    end
end
erp = ads1299eeg.erp_average(corrected,false);
assert(isequal(size(erp.waveform),[250 8]));
fprintf('PASS epochs, baseline and ERP\n');

%% Connectivity
coherence = ads1299eeg.coherence_matrix(data(:,1:4),fs,[8 13],500);
assert(isequal(size(coherence),[4 4]));
assert(max(max(abs(coherence-coherence'))) < 1e-12);
assert(max(abs(diag(coherence)-1)) < 1e-12);
assert(all(coherence(:) >= -1e-9));
assert(all(coherence(:) <= 1+1e-6));
fprintf('PASS coherence matrix\n');

%% Complete pipeline
options = struct('mainsHz',50,'lowHz',0.5,'highHz',45,'applyCAR',false,'nperseg',1000);
result = ads1299eeg.basic_pipeline(data,fs,options);
assert(isequal(size(result.processed),size(data)));
assert(max(abs(result.alphaPeakHz-10)) <= 0.5);
assert(isfield(result,'qualityBefore'));
assert(isfield(result,'qualityAfter'));
assert(isfield(result,'hjorth'));
fprintf('PASS complete pipeline\n');

fprintf('PASS all MATLAB/Octave EEG tests\n');
