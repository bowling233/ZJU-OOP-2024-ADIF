# ADIF Data Process Software

浙江大学《面向对象程序设计》课程实验 1（2023-2024 学年春夏学期，翁恺授课）。

## Build instructions

Note: The `CMakeLists.txt` fetches the third-party libraries automatically from **GitHub**. Make sure you have an internet connection when you build the project for the first time.

```bash
mkdir build
cd build
cmake ..
make -j
```

Two executables will be generated in the `build` directory: `cli` and `tui`. The `tui` executable is currently **under development and is not working functionally**.

## Usage

### Command-line interface (CLI)

```bash
./cli
> help
Usage:
  help: Display this help message.
  read <file>: Read an ADIF file. If there is already data in memory, it will be cleared.
  display [index1 index2 ...]: Display records.
  search <field> <value>: Search records by field. Return indexes of all matched records.
  update <index> <field> <value>: Update records by field.
  delete <index>: Delete records by field.
  merge <file>: Merge another ADIF file into the current data.
  save <file>: Save data to a new ADIF file.
  export <file>: Export data to a CSV file.
  quit: Exit the program.
```

There is a sample input file in the `examples` directory. You can use it to test the program.

```bash
./cli < ../examples/test.in
```

Example:

```text
> # basic
> read ../examples/test0.adif
> display
File: ../examples/test0.adif
Records: 2
<EOH>
<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    1<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:8>JH1GUO/4<FREQ:6>21.013<MODE:2>CW<PFX:3>JH4<QSO_DATE:8>20070526<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>024<STX:1>1<TIME_ON:6>010631<EOR>
<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    2<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:6>JA1MCU<FREQ:6>21.013<MODE:2>CW<PFX:3>JA1<QSO_DATE:8>20070526<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>022<STX:1>2<TIME_ON:6>010813<EOR>
> delete 0
> display
File: ../examples/test0.adif
Records: 1
<EOH>
<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    2<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:6>JA1MCU<FREQ:6>21.013<MODE:2>CW<PFX:3>JA1<QSO_DATE:8>20070526<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>022<STX:1>2<TIME_ON:6>010813<EOR>
> search QSO_DATE 20070526
<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    2<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:6>JA1MCU<FREQ:6>21.013<MODE:2>CW<PFX:3>JA1<QSO_DATE:8>20070526<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>022<STX:1>2<TIME_ON:6>010813<EOR>
> update 0 QSO_DATE 20070527
> display
File: ../examples/test0.adif
Records: 1
<EOH>
<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    2<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:6>JA1MCU<FREQ:6>21.013<MODE:2>CW<PFX:3>JA1<QSO_DATE:8>20070527<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>022<STX:1>2<TIME_ON:6>010813<EOR>
> search QSO_DATE 20070526
> # file
> read ../examples/test1.adif
> display
File: ../examples/test1.adif
Records: 1
<EOH>
<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    1<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:8>JH1GUO/4<FREQ:6>21.013<MODE:2>CW<PFX:3>JH4<QSO_DATE:8>20070526<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>024<STX:1>1<TIME_ON:6>010631<EOR>
> export ../examples/test1.csv
> save ../examples/test1.save.adif
> merge ../examples/test2.adif
Merging...
> display
File: ../examples/test1.adif + ../examples/test2.adif
Records: 3
<EOH>
<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    1<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:8>JH1GUO/4<FREQ:6>21.013<MODE:2>CW<PFX:3>JH4<QSO_DATE:8>20070526<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>024<STX:1>1<TIME_ON:6>010631<EOR>
<CALL:5>JT1CO<FREQ:5>14024<MODE:2>CW<MY_RIG:13>FT847 100W GP<QSLRDATE:6>060606<QSLSDATE:6>060606<QSO_DATE:8>20051231<RST_RCVD:3>599<RST_SENT:3>599<TIME_ON:4>0910<EOR>
<CALL:6>JA7MBT<FREQ:4>7015<MODE:2>CW<MY_RIG:14>FT847
100W INV<NAME:5>TELLY<QSO_DATE:8>20060101<QTH:5>IWAKI<RST_RCVD:3>599<RST_SENT:3>599<TIME_ON:4>0951<EOR>
> merge ../examples/test0.adif
[Warning] Conflicts detected. Please resolve them first.
Conflicts:
Index	Record
--In memory--
0	<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    1<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:8>JH1GUO/4<FREQ:6>21.013<MODE:2>CW<PFX:3>JH4<QSO_DATE:8>20070526<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>024<STX:1>1<TIME_ON:6>010631<EOR>
--To merge--
0	<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    1<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:8>JH1GUO/4<FREQ:6>21.013<MODE:2>CW<PFX:3>JH4<QSO_DATE:8>20070526<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>024<STX:1>1<TIME_ON:6>010631<EOR>
Please enter the indexes of the records to delete:
'o' represent the records in memory, 'n' represent the records to merge.
example: o1 n2 n3
(merge)> o0
Merging...
> display
File: ../examples/test1.adif + ../examples/test2.adif + ../examples/test0.adif
Records: 4
<EOH>
<CALL:5>JT1CO<FREQ:5>14024<MODE:2>CW<MY_RIG:13>FT847 100W GP<QSLRDATE:6>060606<QSLSDATE:6>060606<QSO_DATE:8>20051231<RST_RCVD:3>599<RST_SENT:3>599<TIME_ON:4>0910<EOR>
<CALL:6>JA7MBT<FREQ:4>7015<MODE:2>CW<MY_RIG:14>FT847
100W INV<NAME:5>TELLY<QSO_DATE:8>20060101<QTH:5>IWAKI<RST_RCVD:3>599<RST_SENT:3>599<TIME_ON:4>0951<EOR>
<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    1<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:8>JH1GUO/4<FREQ:6>21.013<MODE:2>CW<PFX:3>JH4<QSO_DATE:8>20070526<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>024<STX:1>1<TIME_ON:6>010631<EOR>
<APP_WRITELOG_COUNTRY:5>Japan<APP_WRITELOG_MULS:5>    2<APP_WRITELOG_P:1>1<APP_WRITELOG_PREF:2>JA<BAND:3>15m<CALL:6>JA1MCU<FREQ:6>21.013<MODE:2>CW<PFX:3>JA1<QSO_DATE:8>20070526<RST_RCVD:3>599<RST_SENT:3>599<SRX:3>022<STX:1>2<TIME_ON:6>010813<EOR>
> save ../examples/merged.adif
> # chinese
> read ../examples/chinese.adif
> display
File: ../examples/chinese.adif
Records: 2
<EOH>
<BAND:9>刚刚好！ <CALL:8>呜呜呜呜<FREQ:6>多了一<MODE:2>哇<QSO_DATE:8>20070526<STX:1>不<TIME_ON:6>010631<EOR>
<BAND:3>！ <CALL:8>呜呜呜呜<FREQ:6>略略略<MODE:2>哇<QSO_DATE:8>20070527<STX:1>？<TIME_ON:6>010631<EOR>
> save ../examples/chinese.save.adif
> # invalid input
> read ../examples/invalid.adif
> display
File: ../examples/invalid.adif
Records: 0
<EOH>
> exit
```

### Text user interface (TUI)

Currently under development.

Design:

```text
 ┌─────────────────────────────────────────────────────────────────────────────────────┐
 │                             ADIF Data Process Software                              │
 ├─────────────────────────────────────────────────────────────────────────────────────┤
 │                                                                                     │
 │ ┌─FileList───┬───────────────────┐   ┌─Filter─────┬───────────┐  ┌─Operation────┐   │
 │ │            │                   │   │   Field    │   Value   │  │ Update       │   │
 │ │ Read       │  test1.adif       │   ├────────────┼───────────┤  │              │   │
 │ │            │                   │   │            │           │  │ Delete       │   │
 │ │ Merge      │  test2.adif       │   │            │           │  │              │   │
 │ │            │                   │   │            │           │  │ Save         │   │
 │ │ Clean      │  test3.adif       │   │            │           │  │              │   │
 │ │            │                   │   │            │           │  │ Export       │   │
 │ │            │  ...              │   │            │           │  │              │   │
 │ │            │                   │   │            │           │  │ Exit         │   │
 │ └────────────┴───────────────────┘   └────────────┴───────────┘  └──────────────┘   │
 │                                                                                     │
 ├────────────┬───────────┬──────────────┬──────────────┬────────────┬─────────────────┤
 │ Field Name │           │              │              │            │                 │
 ├────────────┼───────────┼──────────────┼──────────────┼────────────┼─────────────────┤
 │ Data       │           │              │              │            │                 │
 │            │           │              │              │            │                 │
 │ Data       │           │              │              │            │                 │
 │            │           │              │              │            │                 │
 │ Data       │           │              │              │            │                 │
 │            │           │              │              │            │                 │
 │ ...        │           │              │              │            │                 │
 │            │           │              │              │            │                 │
 └────────────┴───────────┴──────────────┴──────────────┴────────────┴─────────────────┘

```

## Appendix

### Functional requirements

- File reading: The program should be able to read ADIF files with the extension `.adi`.
- Data parsing: Parse the file content and convert the ADIF record into an internal data structure.
- Record display: List all parsed ADIF records.
- Record search: Allow users to search records based on specific fields (such as callsign, date/time, etc.).
- Record update: Allow users to update specific fields of selected records.
- Record deletion: Allow users to delete selected ADIF records.
- Data merging: Allow users to import another ADIF file and merge its data into the data already in memory. Note that there may be records at the same time during this process, and prompt the user whether to overwrite.
- Data export: Able to export the current data to a new ADIF file or other formats (such as CSV).
- User interface: Provide a simple text interface that allows users to interact with the program through text menus.

### Data structure

```text
 ┌───────────────────────────────┐
 │                               │
 │ ┌────────┬─────────┬────────┐ │
 │ │ field  │ length  │ value  │ │
 │ │ name   │         │        │ │
 │ └────────┴─────────┴────────┘ │
 │  adif::Record                 │
 ├───────────────────────────────┤
 │  adif::Record                 │
 ├───────────────────────────────┤
 │  adif::Record                 │
 ├───────────────────────────────┤
 │  adif::Record                 │
 ├───────────────────────────────┤
 │  adif::Record                 │
 └───────────────────────────────┘
   adif::Document
```

### Third-party libraries

- [d99kris/rapidcsv](https://github.com/d99kris/rapidcsv)
- [ArthurSonzogni/FTXUI](https://github.com/ArthurSonzogni/FTXUI)
