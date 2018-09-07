#include "packagedmaterial.h"

#include "cyclus.h"

namespace cyclus {

const ResourceType PackagedMaterial::kType = "PackagedMaterial";

std::map<std::string, int> PackagedMaterial::qualids_;
int PackagedMaterial::next_qualid_ = 1;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PackagedMaterial::Ptr PackagedMaterial::Create(Agent* creator, double quantity,
                             std::string quality) {
  if (qualids_.count(quality) == 0) {
    qualids_[quality] = next_qualid_++;
    creator->context()->NewDatum("PackagedMaterials")
        ->AddVal("QualId", qualids_[quality])
        ->AddVal("Quality", quality)
        ->Record();
  }

  // the next lines must come after qual id setting
  PackagedMaterial::Ptr r(new PackagedMaterial(creator->context(), quantity, quality));
  r->tracker_.Create(creator);
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PackagedMaterial::Ptr PackagedMaterial::CreateUntracked(double quantity,
                                      std::string quality) {
  PackagedMaterial::Ptr r(new PackagedMaterial(NULL, quantity, quality));
  r->tracker_.DontTrack();
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr PackagedMaterial::Clone() const {
  PackagedMaterial* g = new PackagedMaterial(*this);
  Resource::Ptr c = Resource::Ptr(g);
  g->tracker_.DontTrack();
  return c;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void PackagedMaterial::Absorb(PackagedMaterial::Ptr other) {
  if (other->quality() != quality()) {
    throw ValueError("incompatible resource types.");
  }
  quantity_ += other->quantity();
  other->quantity_ = 0;

  tracker_.Absorb(&other->tracker_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PackagedMaterial::Ptr PackagedMaterial::Extract(double quantity) {
  if (quantity > quantity_) {
    throw ValueError("Attempted to extract more quantity than exists.");
  }

  quantity_ -= quantity;

  PackagedMaterial::Ptr other(new PackagedMaterial(ctx_, quantity, quality_));
  tracker_.Extract(&other->tracker_);
  return other;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr PackagedMaterial::ExtractRes(double qty) {
  return boost::static_pointer_cast<Resource>(Extract(qty));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PackagedMaterial::PackagedMaterial(Context* ctx, double quantity, std::string quality)
    : quality_(quality),
      quantity_(quantity),
      tracker_(ctx, this),
      ctx_(ctx) {}

}  // namespace cyclus
