auto Icarus::famicomManifest(string location) -> string {
  vector<uint8> buffer;
  concatenate(buffer, {location, "ines.rom"});
  concatenate(buffer, {location, "program.rom"});
  concatenate(buffer, {location, "character.rom"});
  return famicomManifest(buffer, location);
}

auto Icarus::famicomManifest(vector<uint8>& buffer, string location, uint* prgrom, uint* chrrom) -> string {
  string markup;
  string digest = Hash::SHA256(buffer.data(), buffer.size()).digest();

  if(settings["icarus/UseDatabase"].boolean() && !markup) {
    for(auto node : database.famicom) {
      if(node["sha256"].text() == digest) {
        markup.append(node.text(), "\n  sha256:   ", digest, "\n");
        break;
      }
    }
  }

  if(settings["icarus/UseHeuristics"].boolean() && !markup) {
    FamicomCartridge cartridge{buffer.data(), buffer.size()};
    if(markup = cartridge.markup) {
      markup.append("\n");
      markup.append("information\n");
      markup.append("  title:  ", prefixname(location), "\n");
      markup.append("  sha256: ", digest, "\n");
      markup.append("  note:   ", "heuristically generated by icarus\n");
    }
  }

  auto document = BML::unserialize(markup);
  if(prgrom) *prgrom = document["board/prg/rom/size"].natural();  //0 if node does not exist
  if(chrrom) *chrrom = document["board/chr/rom/size"].natural();  //0 if node does not exist

  return markup;
}

auto Icarus::famicomImport(vector<uint8>& buffer, string location) -> string {
  auto name = prefixname(location);
  auto source = pathname(location);
  string target{settings["Library/Location"].text(), "Famicom/", name, ".fc/"};
//if(directory::exists(target)) return failure("game already exists");

  uint prgrom = 0;
  uint chrrom = 0;
  auto markup = famicomManifest(buffer, location, &prgrom, &chrrom);
  if(!markup) return failure("failed to parse ROM image");
  if(!directory::create(target)) return failure("library path unwritable");

  if(settings["icarus/CreateManifests"].boolean()) file::write({target, "manifest.bml"}, markup);
  file::write({target, "ines.rom"}, buffer.data(), 16);
  file::write({target, "program.rom"}, buffer.data() + 16, prgrom);
  if(!chrrom) return success(target);
  file::write({target, "character.rom"}, buffer.data() + 16 + prgrom, chrrom);
  return success(target);
}
