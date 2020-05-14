//===-- TriCoreELFObjectWriter.cpp - TriCore ELF Writer -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TriCoreFixupKinds.h"
#include "MCTargetDesc/TriCoreMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class TriCoreELFObjectWriter : public MCELFObjectTargetWriter {
public:
  TriCoreELFObjectWriter(uint8_t OSABI);

  ~TriCoreELFObjectWriter() override;
  bool needsRelocateWithSymbol(const MCSymbol &Sym,
                               unsigned Type) const override;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
} // namespace

TriCoreELFObjectWriter::TriCoreELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/* Is64Bit */ false, OSABI, ELF::EM_TRICORE,
                              /*HasRelocationAddend*/ true) {}

TriCoreELFObjectWriter::~TriCoreELFObjectWriter() {}

bool TriCoreELFObjectWriter::needsRelocateWithSymbol(const MCSymbol &Sym,
                                                     unsigned Type) const {
  // FIXME: tricore-ld cannot handle mergeable sections when a section-relative
  //  relocation is used. To get around this problem we force relocations with
  //  symbols for now.
  //  This should be reverted once we have our own linker that can handle
  //  this correctly
  return true;
}

unsigned TriCoreELFObjectWriter::getRelocType(MCContext &Ctx,
                                              const MCValue &Target,
                                              const MCFixup &Fixup,
                                              bool IsPCRel) const {
  // Determine the type of the relocation
  unsigned Kind = Fixup.getTargetKind();
  switch (Kind) {
  default:
    llvm_unreachable("invalid fixup kind!");
  case FK_Data_4:
    return ELF::R_TRICORE_32ABS;
  case TriCore::fixup_24rel:
    return ELF::R_TRICORE_24REL;
  case TriCore::fixup_24abs:
    return ELF::R_TRICORE_24ABS;
  case TriCore::fixup_16sm:
    return ELF::R_TRICORE_16SM;
  case TriCore::fixup_hi:
    return ELF::R_TRICORE_HI;
  case TriCore::fixup_lo:
    return ELF::R_TRICORE_LO;
  case TriCore::fixup_lo2:
    return ELF::R_TRICORE_LO2;
  case TriCore::fixup_18abs:
    return ELF::R_TRICORE_18ABS;
  case TriCore::fixup_15rel:
    return ELF::R_TRICORE_15REL;
  case TriCore::fixup_lha:
    return ELF::R_TRICORE_LHA;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createTriCoreELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<TriCoreELFObjectWriter>(OSABI);
}