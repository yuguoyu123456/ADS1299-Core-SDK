# Brain Regions & EEG Electrode Localization

This directory is the neuroanatomy and scalp-localization knowledge layer of ADS1299-Core-SDK.

The hardware/firmware layers answer **how to acquire EEG reliably**. This layer answers **where the electrodes are, what cortical regions may lie beneath them, and how those regions relate to EEG/BCI experiments**.

## Structure

```text
brain_regions/
├── README.md
├── brodmann_areas/
│   └── README.md              # BA1–BA52 learning/index layer
├── eeg_electrode_systems/
│   └── README.md              # 10–20 / 10–10 / 10–5 systems
└── electrode_cortex_mapping/
    └── README.md              # scalp electrode ↔ cortex/BA mapping rules
```

## Why this belongs in an ADS1299 project

ADS1299 is an acquisition front end. In EEG and BCI, the final scientific question is usually not only whether eight channels were sampled correctly, but **what neural system the montage is intended to observe**.

A complete workflow therefore connects:

```text
ADS1299 channel
    ↓
physical electrode
    ↓
10–20 / 10–10 / 10–5 scalp position
    ↓
approximate underlying cortical anatomy
    ↓
Brodmann / anatomical / functional-network interpretation
    ↓
EEG feature / ERP / BCI paradigm
```

This directory is intended to become the bridge between acquisition engineering and neuroscience interpretation.

## Three knowledge layers

### 1. Brodmann areas

[`brodmann_areas/`](brodmann_areas/) organizes the classical Brodmann cortical parcellation and will progressively cover BA1–BA52 with anatomy, major functional associations, common experimental tasks, EEG accessibility, and important interpretation limits.

### 2. EEG electrode placement systems

[`eeg_electrode_systems/`](eeg_electrode_systems/) covers standardized scalp nomenclature and placement systems, beginning with:

- International 10–20 system;
- 10–10 system;
- 10–5 high-density extension.

### 3. Electrode-to-cortex mapping

[`electrode_cortex_mapping/`](electrode_cortex_mapping/) connects scalp positions such as F3, C3, Cz, Pz, O1/O2, etc. to likely underlying cortical regions while explicitly preserving the key scientific limitation:

> **A scalp electrode is not a one-to-one label for one Brodmann area.**

Head shape, electrode placement error, cortical folding, conductivity, reference choice and volume conduction all affect the measured signal. Individual MRI/source localization is required when precise cortical localization matters.

## Source policy

This knowledge layer should be evidence-first. Prefer:

1. recognized EEG electrode-position standards/guidelines;
2. canonical electrode-system papers;
3. primary neuroanatomy/cytoarchitecture sources and modern atlases;
4. peer-reviewed MRI/electrode localization studies;
5. major neuroscience/EEG textbooks and authoritative reviews.

Do not present a convenient electrode→Brodmann lookup as exact anatomy when the literature only supports an approximate scalp-to-cortex relationship.

## Initial references

- Jasper HH. *The ten-twenty electrode system of the International Federation.* Electroencephalography and Clinical Neurophysiology. 1958;10:371–375.
- American Electroencephalographic Society. *Guidelines for standard electrode position nomenclature.* Journal of Clinical Neurophysiology. 1991;8(2):200–202. PMID: 2050819.
- Acharya JN, et al. *American Clinical Neurophysiology Society Guideline 2: Guidelines for Standard Electrode Position Nomenclature.* Journal of Clinical Neurophysiology. 2016. DOI: 10.1097/WNP.0000000000000316.
- Oostenveld R, Praamstra P. *The five percent electrode system for high-resolution EEG and ERP measurements.* Clinical Neurophysiology. 2001;112(4):713–719. DOI: 10.1016/S1388-2457(00)00527-7.
- Homan RW, Herman J, Purdy P. *Cerebral location of international 10-20 system electrode placement.* Electroencephalography and Clinical Neurophysiology. 1987. PMID: 2435517.

## Long-term direction

Future maintenance can deepen this area one leaf at a time: one Brodmann area, one electrode family, one montage, one electrode-cortex mapping question, or one BCI paradigm-specific placement guide. The goal is a scientifically careful learning layer rather than an oversized unverified table.