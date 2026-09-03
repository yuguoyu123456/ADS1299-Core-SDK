# EEG Electrode Placement Systems

This subdirectory documents standardized scalp-electrode positioning systems used in EEG/ERP/BCI.

## Core systems

### International 10–20 system

The classical 10–20 system uses reproducible cranial landmarks and proportional distances to define standard scalp positions. It remains the basic vocabulary for clinical EEG, research EEG and many BCI montages.

Common labels include families such as:

- `Fp` — frontopolar;
- `F` — frontal;
- `C` — central;
- `T` — temporal;
- `P` — parietal;
- `O` — occipital;
- `z` — midline;
- odd numbers — left hemisphere;
- even numbers — right hemisphere.

Modern nomenclature has changed some older temporal labels in common use, so future pages should distinguish legacy and current naming carefully.

### 10–10 system

The 10–10 system increases spatial sampling density by adding intermediate positions between classical 10–20 locations. It is widely used for higher-channel-count EEG, ERP and BCI systems.

For this project, 10–10 is especially important for 32/64-channel ADS1299 systems because it provides a standardized framework for channel naming and cap design rather than an arbitrary electrode list.

### 10–5 system

The 10–5 system is a further high-density extension. Oostenveld and Praamstra described a five-percent system extending the 10–20/10–10 framework to hundreds of standardized positions for high-resolution EEG/ERP work.

It is useful as a reference even when the actual ADS1299 system has far fewer channels, because it provides a common coordinate/naming vocabulary for dense montages and electrode subsets.

## What this folder should eventually contain

```text
eeg_electrode_systems/
├── README.md
├── 10_20/
│   └── README.md
├── 10_10/
│   └── README.md
├── 10_5/
│   └── README.md
├── naming_conventions/
│   └── README.md
└── montage_examples/
    └── README.md
```

The child folders above are the planned structure. They should be created when each becomes a real, evidence-backed deliverable rather than as empty placeholders.

## Future content for each system

A mature page should include:

1. cranial landmarks and measurement method;
2. naming rules;
3. standard positions and diagrams that can be legally redistributed or independently generated;
4. differences from neighboring systems;
5. common channel-count subsets (8/16/19/21/32/64/128 where relevant);
6. ADS1299 channel-to-electrode naming examples;
7. typical EEG/BCI use cases;
8. reference/ground/electrode-cap implementation notes;
9. legacy-vs-current nomenclature warnings;
10. links to electrode→cortex interpretation pages.

## ADS1299 system-design connection

For a multi-ADS1299 project, channel numbers such as `CH1`–`CH64` should not be the final scientific identity of the data. Acquisition metadata should eventually preserve a mapping such as:

```text
ADS1299 #1 / CH1  → Fp1
ADS1299 #1 / CH2  → Fp2
...
ADS1299 #8 / CH8  → montage-defined electrode
```

The exact mapping is experiment-specific; the important engineering rule is that channel order, physical connector order, cap label and exported data metadata must stay consistent.

## Core references

- Jasper HH. *The ten-twenty electrode system of the International Federation.* Electroencephalography and Clinical Neurophysiology. 1958;10:371–375.
- American Electroencephalographic Society. *Guidelines for standard electrode position nomenclature.* Journal of Clinical Neurophysiology. 1991;8(2):200–202. PMID: 2050819.
- Acharya JN, et al. *American Clinical Neurophysiology Society Guideline 2: Guidelines for Standard Electrode Position Nomenclature.* Journal of Clinical Neurophysiology. 2016. DOI: 10.1097/WNP.0000000000000316.
- Oostenveld R, Praamstra P. *The five percent electrode system for high-resolution EEG and ERP measurements.* Clinical Neurophysiology. 2001;112(4):713–719. DOI: 10.1016/S1388-2457(00)00527-7.

## Scientific caution

Electrode labels standardize scalp positions. They do **not** by themselves prove the cortical generator of an EEG feature. Use the electrode-position system together with anatomy, source-localization evidence and the experimental paradigm.