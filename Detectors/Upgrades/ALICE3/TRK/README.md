<!-- doxy
\page refDetectorsUpgradesALICE3TRK Tracker
/doxy -->

# ALICE 3 Tracker Barrel

This is top page for the TRK detector documentation.


## Specific detector setup


Configurables for various sub-detectors are presented in the following Table:

| Subsystem          | Available options                                       | Comments                                                         |
| ------------------ | ------------------------------------------------------- | ---------------------------------------------------------------- |
| `TRKBase.layoutVD` | `kIRIS4` (default), `kIRISFullCyl`, `kIRIS5`, `kIRIS4a` | [link to definitions](./base/include/TRKBase/TRKBaseParam.h) |
| `TRKBase.layoutMLOT` | `kCylindrical`, `kSegmented` (default), `kSimplifiedRealistic` | `kCylindrical`: simple silicon tubes. `kSegmented`: Turbo ML + solid-module OT. `kSimplifiedRealistic`: same ML as `kSegmented`, but a detailed OT barrel (see below) |
| `TRKBase.layoutSRV` | `kPeacockv1` (default), `kLOISymm` | `kLOISymm` produces radially symmetric service volumes, as used in the LoI |

For example, a geometry with fully cylindrical tracker barrel (for all layers in VD, ML and OT) can be obtained by
```bash
o2-sim-serial-run5 -n 1 -g pythia8hi -m A3IP TRK FT3 TF3 \
  --configKeyValues "TRKBase.layoutVD=kIRISFullCyl;TRKBase.layoutMLOT=kCylindrical"
```

## Custom Geometry Configuration

The geometry of the ML and OT layers can be overridden by providing a custom plain-text configuration file via `TRKBase.configFile=filename.txt`. The parser interprets the file differently depending on the active `TRKBase.layoutMLOT` setting (`kCylindrical`, or `kSegmented`/`kSimplifiedRealistic` which share the same syntax).

### General Syntax Rules
* **Separators:** All columns **must** be separated by a single TAB (`\t`). Using spaces will result in a parsing error.
* **Comments:** Any line starting with a forward slash (`/`) is treated as a comment and ignored.
* **Layer Count:** The parser reads valid lines sequentially. The first valid line corresponds to Layer 0, the second to Layer 1, and so on.
* **Material Budget Mode:** All layer definitions accept an optional `matBudgetMode` parameter at the end of the line (e.g., `0` = Thickness, `1` = X2X0). If omitted, it defaults to `Thickness`.

### 1. Cylindrical Layout (`kCylindrical`)

When `TRKBase.layoutMLOT=kCylindrical` is used, each layer requires a minimum of 3 parameters to define the `TRKCylindricalLayer`.

* **Format:** `rInn` \t `length` \t `thick` \t `[optional_mode]`
* *(Note: `rInn`, `length`, and `thick` map directly to the constructor arguments for the cylindrical layer, typically corresponding to Radius, Length, and Thickness).*

**Example for `kCylindrical`:**
```text
/ Configuration for kCylindrical layout - ALICE3 TRK
/ rInn	length	thick	[optional_mode]
7.0	127.985	0.1
9.0	127.985	0.1
12.0	127.985	0.1
20.0	127.985	0.1
30.0	127.985	0.1
45.0	255.9	0.1
60.0	255.9	0.1
80.0	255.9	0.1
```

### 2. Segmented / Simplified-Realistic Layout (`kSegmented`, `kSimplifiedRealistic`)

Both layouts use the same configuration-file syntax (only the OT geometry implementation differs). Each layer requires a minimum of 5 base parameters to define the geometry. The parser distinguishes between Middle Layers (ML) and Outer Layers (OT) based on the sequential layer index.

* *(Note: The 5 base parameters map directly to: Inner Radius (`rInn`), Thickness (`thick`), Tilt Angle (`tiltAngle`), Number of Staves (`nStaves`), and Number of Modules per stave (`nMods`)).*

**Middle Layers (ML) - Indices 0 to 4**
The first 5 valid lines are parsed as `TRKMLLayer` objects. These layers **require** a 6th parameter for the staggering offset (`stagOffset`).
* **Format:** `rInn` \t `thick` \t `tiltAngle` \t `nStaves` \t `nMods` \t `stagOffset` \t `[optional_mode]`

**Outer Layers (OT) - Indices 5 and above**
From the 6th valid line onwards, lines are parsed as OT layer objects (`TRKOTLayer` for `kSegmented`, `TRKOTLayerRealistic` for `kSimplifiedRealistic`). These layers do **not** have a staggering offset. The optional mode parameter shifts to the 6th column.
* **Format:** `rInn` \t `thick` \t `tiltAngle` \t `nStaves` \t `nMods` \t `[optional_mode]`
* *(Note: for `kSimplifiedRealistic`, `nStaves` is recomputed internally from the average radius and stave width to guarantee the neighbour overlap; the value in the file is ignored for the OT.)*

**Example for `kSegmented`:**

```text
/ Configuration for kSegmented layout - ALICE3 TRK
/ --- ML LAYERS (Indices 0 to 4) ---
/ rInn	thick	tilt	nStaves	nMods	stagOffset	[optional_mode]
7.0	0.01	11.2	10	11	0.0	1
9.0	0.01	11.9	14	11	0.0	1
12.0	0.01	11.4	18	11	0.0	1
20.0	0.01	0.0	26	11	1.17	1
30.0	0.01	0.0	38	11	0.89	1
/
/ --- OT LAYERS (Indices 5 to 7) ---
/ Outer layers do NOT have stagOffset.
/ rInn	thick	tilt	nStaves	nMods	[optional_mode]
45.0	0.01	0.0	32	22	1
60.0	0.01	0.0	42	22	1
80.0	0.01	0.0	56	22	1
```

## Simplified-Realistic OT geometry (`kSimplifiedRealistic`)

`kSimplifiedRealistic` keeps the ML layers identical to `kSegmented` but replaces the solid-silicon OT modules with a more detailed, but still simplified, description (`TRKOTLayerRealistic`). All tunable dimensions live in [`Specs.h`](./base/include/TRKBase/Specs.h) (`constants::OT`); values that depend on others are computed in the source.

**Module** — a flush stack about the chip mid-plane: cold plate (carbon fibre), 8 pure-silicon chips (2 in φ × 4 in z, dead zones facing the outer module edges), FPC (Kapton+Cu), one ZIF connector centred on a short edge, SMD capacitors over the chip footprints (skipping any under the connector), and two mounting brackets on the cold plate.

**Stave** — two module rows overlapping in φ (so each row's dead zone is covered by the other row's sensor) and offset in r, plus a cooling pipe between them.

**Barrel** — each layer is built in **four parts**: two z-half-barrels (±η), each split azimuthally into two 180° halves cut on **perpendicular planes** (one horizontal, one vertical, matching the Peacock services). Neighbouring staves overlap (≥ 1 mm active double-coverage); the two azimuthal halves meet at a ~1 mm gap instead. The stave count is fixed (the gap is absorbed into a slightly larger internal overlap). The cooling pipe faces the larger radius on the inner two OT layers (R = 45, 60 cm) and the smaller radius on the flipped outer layer (R = 80 cm).

The OT services (cables/cooling bundles, cold plates) are built by `TRKServices` for all layouts via `TRKBase.layoutSRV`.

<!-- doxy
/doxy -->
