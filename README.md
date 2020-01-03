# Cumbia access to the  *hdb* and *hdb++* historical databases

The *cumbia-historicaldb* library provides access to the *hdb* and *hdbpp* historical databases.
In the distribution you will find the following components:

- the C++ library, requiring the *meson* build system and the cumbia and *hdbextractor* dependencies.
  The latter can be downloaded from https://github.com/ELETTRA-SincrotroneTrieste/hdbextractor.git

- A Qt library, that simply defines a *reader* and a *factory* that allow integration into a cumbia/Qt
  application, so that a *setSource* call with a special syntax can trigger a database data fetch operation.

- A Qt *plugin*, that enables the programmer to load the *cumbia-historicaldb* module as a plugin

- A Qt application, named *qhdbbrowser*, to query the database for stored scalar data and display it
  on a graph

- A Qt application, named *hdb-db-profile-manager*, to create, remove and modify database *profiles* (see
  the dedicated section below)

## Installation

- git clone https://github.com/ELETTRA-SincrotroneTrieste/hdbextractor.git

- cd cumbia-historicaldb

### Base C++ library

- meson builddir
- cd builddir

#### Note
Use as prefix the same path used for the cumbia-libs installation, so that there is no need to
modify the LD_LIBRARY_PATH and the QT_PLUGIN_PATH later:

- meson configure -Dprefix=/usr/local/cumbia-libs
- meson configure -Dlibdir=lib  if you want to install libs under *prefix/lib* instead of *prefix/lib64*
- meson configure -Dbuildtype=release|debug

ninja && ninja install

### Qt module

- cd cumbia-historicaldb-qt

There is a top level Qt *pro* file that will build the *Qt library*, the *Qt plugin* and the two aforementioned
applications.

Invoke qmake with the INSTALL_ROOT option matching both the *prefix* specified in the *meson configure* command above
and the cumbia install prefix:

- qmake INSTALL_ROOT=/usr/local/cumbia-libs
- make && make install

Plugins will be installed under *INSTALL_ROOT/lib/qumbia-plugins*, alongside the other *cumbia qt plugins* either from the
main cumbia distribution or from optional ones.

## Database profiles

An application connecting to a database needs to know the database host name, port, user name, password, database
schema and so on. One usually specifies all these options either through the command line or a configuration file.
*cumbia-historicaldb* makes things easier introducing *database profiles*.
The profiles contain the information needed to connect to a historical database through the *hdbextractor* library.
Every application relying on *cumbia-historicaldb* will just need a profile name to use it. Additionally, if no
specific profile is given, a default one is used. If only one profile is defined, it is also the default.
The hdb-db-profile-manager utility described later can be used to mark a profile as default. Adding a new profile does
not alter the default one. This means that if a *second* profile is defined, the *first* still is the default.


Profiles are stored in the user's directory under *.config/cumbia-hdb/db-profiles* or where specified in the file
*cuhdb_config.h* under the *lib/* folder of this distribution.

If the *cumbia-historicaldb-qt* package is compiled and installed as explained before, you can generate, edit and
delete profiles with the utility *hdb-db-profile-manager*. Type the command without any parameter to obtain the help.

### Examples

#### Generate a new profile named hdb_profile1

> hdb-db-profile-manager  -a  hdb_profile1

Follow the instructions

#### List available profiles

> hdb-db-profile-manager -l

#### Show the configuration of a given profile

> hdb-db-profile-manager hdb_profile1

#### Remove a given profile

> hdb-db-profile-manager -r hdb_profile1

#### Edit an existing profile

hdb-db-profile-manager -e hdb_profile1
