# This Ghidra script scans the Black Mesa Linux `server_srv.so` binary and does the following:
#
# - Writes vftables, datamaps and netprops to all corresponding classes
#   - You will need to dump the datamap/netprops to disk as a XML file via `sm_dump_datamaps_xml` and `sm_dump_netprops_xml`.
# - Writes to `gpGlobals` and `g_pGameRules` with their respective structures
# - Parses the binary for the list of activities.

from ghidra.program.model.data import (
    StructureDataType,
    CategoryPath,
    DataTypeConflictHandler,
    PointerDataType,
    BooleanDataType,
    FloatDataType,
    ArrayDataType,
    CharDataType,
    ShortDataType,
    IntegerDataType,
    UnsignedCharDataType,
    UnsignedShortDataType,
    UnsignedIntegerDataType,
    EnumDataType,
)
from ghidra.app.decompiler import (
    DecompInterface,
    DecompileOptions,
)
import ghidra.program.model.symbol.SourceType as SourceType
import ghidra.program.model.pcode.PcodeOp as PcodeOp
import docking.widgets.filechooser.GhidraFileChooser as GhidraFileChooser
import xml.etree.ElementTree as ET

dtm = currentProgram.getDataTypeManager()
sm = currentProgram.getSymbolTable()

class StructVftable:
    def __init__(self, symbol):
        self.name = symbol.getParentSymbol().getName()
        self.vftable = []

        address = symbol.getAddress().add(8)
        while True:
            try:
                fn = getFunctionAt(getDataAt(address).getValue())
                name = fn.getName()

                # Avoid naming conflicts by appending.
                count = 0
                for e in self.vftable:
                    if e["name"].startswith(name):
                        count += 1
                if count > 0:
                    name += "_" + str(count)

                self.vftable.append({
                    "fn": fn,
                    "name": name,
                })
            except:
                break
            address = address.add(4)
        
        if len(self.vftable) == 0:
            raise BaseException("Obtained `0` entries for vftable `{}`.".format(self.name))

class StructVftTableList:
    def __init__(self):
        self.structs = []

        monitor.setIndeterminate(True)
        monitor.setMessage("Parsing vftables...")

        symbols = list(sm.getAllSymbols(False))
        monitor.initialize(len(symbols))

        for symbol in symbols:
            monitor.checkCancelled()
            monitor.incrementProgress(1)

            if symbol.getName() == "vtable":
                try:
                    self.structs.append(StructVftable(symbol))
                except:
                    continue
                struct = self.structs[len(self.structs) - 1]
                monitor.setMessage("Parsed `{}` vftable entries for `{}`...".format(len(struct.vftable), struct.name))

class StructDatamap:
    def __init__(self, serverclass):
        self.name = serverclass.get("element").replace("_", "").lower()
        self.variables = []
        self._parse(serverclass, 0)
        self.variables.sort(key=lambda datamap: datamap["offset"])
    def _parse(self, table, tableoffset):
        for datamap in table.findall("datamap"):
            name = datamap.get("name")
            offset = int(datamap.find("offset").text) 
            size = int(datamap.find("bytes").text)
            
            # Fixes issues with m_vec...[] overlapping with m_vec....
            if name.endswith("]"):
                continue
            
            # Skip entries with a size of 0.
            # These are usually function pointers without a size or offset.
            if size == 0:
                continue
                
            # Skip entries that overlap with the vftable.
            if tableoffset + offset <= 4:
                continue
            
            if any(name == e["name"] for e in self.variables):
                continue
            
            try:
                self.variables.append({
                    "name": name,
                    "offset": tableoffset + offset,
                    "type": StructDatamap._parse_to_ghidra_type(name, size),
                })
            except:
                continue
        
        for subtable in table.findall("subtable"):
            offset = int(subtable.get("offset"))

            # Skip entries that overlap with the vftable.
            if offset <= 4:
                continue
            
            self._parse(subtable, tableoffset + offset)
    @staticmethod
    def _parse_to_ghidra_type(name, size):
        if name.startswith("m_isz") or name.startswith("m_sz") or name.startswith("m_str"):
            return PointerDataType(CharDataType())
        elif name.startswith("m_pfn"):
            return PointerDataType()
        if name.startswith("m_b"):
            return BooleanDataType()
        elif name.startswith("m_fl"):
            return FloatDataType()
        elif name.startswith("m_vec") or name.startswith("m_ang"):
            return ArrayDataType(FloatDataType(), 3, 4)
        elif name.startswith("m_h"):
            return IntegerDataType()
        elif name.startswith("m_i"):
            if size == 1:
                return CharDataType()
            elif size == 2:
                return ShortDataType()
            elif size == 4:
                return IntegerDataType()
        elif name.startswith("m_n") or name.startswith("m_f"):
            if size == 1:
                return UnsignedCharDataType()
            elif size == 2:
                return UnsignedShortDataType()
            elif size == 4:
                return UnsignedIntegerDataType()
        elif name.startswith("m_p"):
            return PointerDataType()
        raise BaseException("Could not parse datamap variable `{}`.".format(name))

class StructDatamapList:
    def __init__(self, path):
        self.structs = []

        monitor.setIndeterminate(True)
        monitor.setMessage("Parsing datamaps...")

        serverclasses = ET.parse(path).getroot()
        monitor.initialize(len(serverclasses))
        
        for serverclass in serverclasses:
            monitor.checkCancelled()
            monitor.incrementProgress(1)

            self.structs.append(StructDatamap(serverclass))
            struct = self.structs[len(self.structs) - 1]
            monitor.setMessage("Parsed `{}` datamap entries for `{}`...".format(len(struct.variables), struct.name))

class StructNetprop:
    def __init__(self, serverclass):
        self.name = serverclass.get("name")
        self.variables = []
        self._parse(serverclass.find("sendtable"), 0)
        self.variables.sort(key=lambda netprop: netprop["offset"])
    def _parse(self, table, tableoffset):
        for property in table.findall("property"):
            name = property.get("name")
            type = property.find("type").text
            offset = int(property.find("offset").text)
            bits = int(property.find("bits").text)
            flags = property.find("flags").text or ""

            # Inheritance.
            if name == "baseclass":
                self._parse(property.find("sendtable"), 0)
            # This is a data table containing other offsets.
            elif type == "datatable":
                ## Check whether offset overlaps with vftable.
                #if offset != 0 and offset <= 4:
                #    print("Skipped table " + name)
                #    continue
                
                self._parse(property.find("sendtable"), tableoffset + offset)
            # This is a member variable.
            else:
                # Fixes issues with m_vec...[] overlapping with m_vec....
                if name.endswith("]"):
                    continue

                # Skip names that are only digits.
                # TODO: add support for subclasses.
                if name.isdigit():
                    continue

                # Remove duplicates.
                if any(name == e["name"] for e in self.variables):
                    continue

                # Check whether offset overlaps with vftable or is an invalid offset.
                if tableoffset + offset <= 4:
                    #print("Skipped " + name)
                    continue

                try:
                    self.variables.append({
                        "name": name,
                        "offset": tableoffset + offset,
                        "type": StructNetprop._parse_to_ghidra_type(name, type, bits, flags),
                    })
                except:
                    continue
    @staticmethod
    def _parse_to_ghidra_type(name, type, bits, flags):
        if name.startswith("m_fl") or type == "float":
            return FloatDataType()
        elif name.startswith("m_vec") or type == "vector":
            return ArrayDataType(FloatDataType(), 3, 4)
        elif type == "string":
            return PointerDataType(ghidra.program.model.data.CharDataType())
        elif type == "integer":
            if name.startswith("m_b"):
                return BooleanDataType()
            if bits > 0 and bits <= 32:
                unsigned = flags.find("Unsigned") != -1
                if bits <= 8:
                    return UnsignedCharDataType() if unsigned else data.CharDataType()
                elif bits <= 16:
                    return UnsignedShortDataType() if unsigned else ShortDataType()
                elif bits <= 32:
                    return UnsignedIntegerDataType() if unsigned else IntegerDataType()
        raise BaseException("Could not determine type `" + type + "` for netprop variable `" + name + "`.")

class StructNetpropList:
    def __init__(self, path):
        self.structs = []

        monitor.setIndeterminate(True)
        monitor.setMessage("Parsing netprops...")

        serverclasses = ET.parse(path).getroot()
        monitor.initialize(len(serverclasses))

        for index, serverclass in enumerate(serverclasses):
            monitor.checkCancelled()
            monitor.incrementProgress(1)

            self.structs.append(StructNetprop(serverclass))
            struct = self.structs[len(self.structs) - 1]
            monitor.setMessage("Parsed `{}` netprop entries for `{}`...".format(len(struct.variables), struct.name))

def write_variable(datatype, variable):
    end = variable["offset"] + variable["type"].getLength()
    if datatype.getLength() < end:
        datatype.growStructure(end - datatype.getLength())
    datatype.replaceAtOffset(variable["offset"], variable["type"], 0, variable["name"], "")

def write_properties(vftables, datamaps, netprops):
    monitor.setIndeterminate(True)
    monitor.setMessage("Writing vftables, datamaps and netprops...")

    datatypes = list(dtm.getAllDataTypes())
    monitor.initialize(len(datatypes))

    for datatype in datatypes:
        monitor.checkCancelled()
        monitor.incrementProgress(1)

        name = datatype.getName()
        short_name = datatype.getName().lower().replace("_", "")

        if vftables is not None:
            for struct in vftables.structs:
                if struct.name == datatype.name:
                    monitor.setMessage("Writing `{}` vftable entries for `{}`...".format(len(struct.vftable), struct.name))

                    vftable_datatype = dtm.addDataType(StructureDataType(datatype.getCategoryPath(), struct.name + "_vftable", 0), DataTypeConflictHandler.REPLACE_HANDLER)
                    for e in struct.vftable:
                        vftable_datatype.add(PointerDataType()).setFieldName(e["name"])
                        e["fn"].setCallingConvention("__thiscall")
                    
                    datatype.deleteAll()
                    write_variable(datatype, {
                        "name": "vftable",
                        "offset": 0,
                        "type": PointerDataType(vftable_datatype),
                    })
                    
                    break
        
        if datamaps is not None:
            for struct in datamaps.structs:
                if struct.name == short_name or struct.name == short_name[1:]:
                    monitor.setMessage("Writing `{}` datamap entries for `{}`...".format(len(struct.variables), struct.name))

                    for variable in struct.variables:
                        write_variable(datatype, variable)
                    
                    break
        
        if netprops is not None:
            for struct in netprops.structs:
                if struct.name == datatype.name:
                    monitor.setMessage("Writing `{}` netprop entries for `{}`...".format(len(struct.variables), struct.name))

                    for variable in struct.variables:
                        write_variable(datatype, variable)
                    
                    break

class ActivityList:
    def __init__(self):
        self.variants = []

        monitor.setIndeterminate(True)
        monitor.setMessage("Parsing activities...")
        
        options = DecompileOptions()
        ifc = DecompInterface()
        ifc.setOptions(options)
        ifc.openProgram(currentProgram)
        ifc.setSimplificationStyle("decompile")
        
        for symbol in sm.getAllSymbols(False):
            if symbol.getName() == "ActivityList_RegisterSharedActivities":
                res = ifc.decompileFunction(getFunctionContaining(symbol.getAddress()), 30, monitor)
                pcodes = list(res.getHighFunction().getPcodeOps())

                for pcode in pcodes:
                    if pcode.getOpcode() != PcodeOp.CALL:
                        continue

                    if getSymbolAt(pcode.getInput(0).getAddress()).getName() == "ActivityList_RegisterSharedActivity":
                        name = getDataAt(toAddr(pcode.getInput(1).getDef().getInput(0).getAddress().getUnsignedOffset()))

                        # TODO:
                        # There are some strings that Ghidra may not catch and will not define.
                        # Manually define them in this branch.
                        if name is None:
                            continue

                        self.variants.append({
                            "name": name.getDefaultValueRepresentation().strip("\""),
                            "value": pcode.getInput(2).getAddress().getUnsignedOffset(),
                        })

                        variant = self.variants[len(self.variants) - 1]
                return
        
        raise BaseException("Could not find `ActivityList_RegisterSharedActivities`...")
    def write(self):
        monitor.setIndeterminate(True)
        monitor.setMessage("Creating `Activity` enum structure...")

        datatype = EnumDataType(CategoryPath("/Demangler"), "Activity", 4)
        for variant in self.variants:
            datatype.add(variant["name"], variant["value"])
        datatype = dtm.addDataType(datatype, DataTypeConflictHandler.REPLACE_HANDLER)
        
        monitor.setMessage("Setting function return types to `Activity`...")

        symbols = list(sm.getAllSymbols(True))
        monitor.initialize(len(symbols))

        for symbol in symbols:
            monitor.checkCancelled()
            monitor.incrementProgress(1)

            if symbol.getName().startswith("Get") and symbol.getName().endswith("Activity"):
                fn = getFunctionAt(symbol.getAddress())
                if fn is not None:
                    fn.setReturnType(datatype, SourceType.USER_DEFINED)
                    monitor.setMessage("Changed function `{}` return type to `Activity`...".format(symbol.getName(True)))

def write_gp_globals():
    monitor.setIndeterminate(True)
    monitor.setMessage("Retyping `gpGlobals` to `CGlobalVars`...")

    # TODO:
    # Add the rest of the fields and add comments to the fields.
    gp_global_datatype = dtm.addDataType(StructureDataType(CategoryPath("/Demangler"), "CGlobalVars", 0), DataTypeConflictHandler.REPLACE_HANDLER)
    gp_global_datatype.add(FloatDataType()).setFieldName("realtime")
    gp_global_datatype.add(IntegerDataType()).setFieldName("framecount")
    gp_global_datatype.add(FloatDataType()).setFieldName("absoluteframetime")
    gp_global_datatype.add(FloatDataType()).setFieldName("curtime")
    gp_global_datatype.add(FloatDataType()).setFieldName("frametime")
    gp_global_datatype.add(IntegerDataType()).setFieldName("maxClients")
    gp_global_datatype.add(IntegerDataType()).setFieldName("tickcount")
    gp_global_datatype.add(FloatDataType()).setFieldName("interval_per_tick")
    gp_global_datatype.add(FloatDataType()).setFieldName("interpolation_amount")
    gp_global_datatype.add(IntegerDataType()).setFieldName("simTicksThisFrame")
    gp_global_datatype.add(IntegerDataType()).setFieldName("network_protocol")
    for symbol in sm.getAllSymbols(True):
        if symbol.getName() == "gpGlobals":
            removeDataAt(symbol.getAddress())
            createData(symbol.getAddress(), PointerDataType(gp_global_datatype))
            break

def write_gamerules():
    monitor.setIndeterminate(True)
    monitor.setMessage("Retyping `g_pGameRules` to `CBM_MP_GameRules`...")

    gamerules_datatype = dtm.getDataType("/Demangler/CBM_MP_GameRules")
    for symbol in sm.getAllSymbols(True):
        if symbol.getName() == "g_pGameRules":
            removeDataAt(symbol.getAddress())
            createData(symbol.getAddress(), PointerDataType(gamerules_datatype))
            break

chooser = GhidraFileChooser(None)
chooser.setTitle("select xml datamaps file (dumped with `sm_dump_datamaps_xml`)")
path_datamap = chooser.getSelectedFile().getPath()
chooser.setTitle("select xml netprops file (dumped with `sm_dump_netprops_xml`)")
path_netprop = chooser.getSelectedFile().getPath()

write_properties(StructVftTableList(), StructDatamapList(path_datamap), StructNetpropList(path_netprop))
ActivityList().write()
write_gp_globals()
write_gamerules()

# TODO:
# - Change all functions return type to bool if function name starts with "Is", "Can", "Will", "Has" and has a leading upper case letter

# TODO:
# Retype the 2nd argument to char* of the following:
# - SendPropInt
# - SendPropFloat
# - SendPropBool
# - SendPropVector
