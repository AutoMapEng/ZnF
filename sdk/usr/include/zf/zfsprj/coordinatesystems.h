#pragma once

#include "ZfsPrjBasic.h"

#include <map>
#include <memory>
#include <string>

#include "tools.h"
#include "xml.h"
#include <zfs/trmatrix.h>

// interface for coordinate systems
#define NULL_SYSTEM L"Default"
#define UTM_SYSTEM L"UTM"
#define TACHY_SYSTEM L"Tachy"
#define PLAN_SYSTEM L"Annotations"

namespace zfprj {

 /**
 * @brief A single coordinate system.
 */
class ZFSPRJ_API CoordinateSystem {

private:
    std::shared_ptr<ZFTRMatrix> matrix{};
    std::wstring name;

    bool deleted{};

public:
    CoordinateSystem();

    CoordinateSystem(const std::wstring &name_);

    CoordinateSystem(const std::wstring &name_, ZFTRMatrix &_matrix);


    const std::wstring &getName() const;

    void setDeleted(bool _deleted);

    bool isDeleted() const;

    void setMatrix(ZFTRMatrix &_matrix);

    const std::shared_ptr<ZFTRMatrix> getMatrix();
};

/**
 * @brief Manage all coordinate systems.
 */
class ZFSPRJ_API CoordinateSystems {

private:
  /**
   * @brief Map containing all available systems
   */
  std::map<std::wstring, std::shared_ptr<zfprj::CoordinateSystem>> systemMap;

  /**
   * @brief Name of the current system
   */
  std::wstring currentSelected;

  double projectAngleToNorth;

public:
  CoordinateSystems() : currentSelected(NULL_SYSTEM) { clear(); }

  /**
   * @brief Remove all systems and add default systems.
   */
  void clear();

  /**
   * @brief Add a new system.
   *
   * @param system the system to be added
   */
  void addSystem(std::shared_ptr<zfprj::CoordinateSystem> system);

  /**
   * @brief Mark the system with the given name as deleted.
   *
   * @param name name of the system to be deleted
   */
  void deleteSystem(const std::wstring &name);

  /**
   * @brief Set the active system. This does not change the global matrix
   *        in the project. This has to be done afterwards.
   *
   * @param name name of the current system.
   *
   * @return true, if the system was found
   */
  bool setCurrentSystem(const std::wstring &name);

  /// get the current selected system
  std::shared_ptr<zfprj::CoordinateSystem> getSelectedSystem();

  /**
   * @brief Get the zero system without any values.
   */
  std::shared_ptr<zfprj::CoordinateSystem> getNullSystem();

  /**
   * @brief Get a system by its name.
   */
  std::shared_ptr<zfprj::CoordinateSystem>
  getSystemByName(const std::wstring &name);

  /**
   * @brief Get a list of all system names.
   *
   * @return the list
   */
  std::list<std::wstring> getSystemNames();

  /**
   * @brief Check wheter a gps system is available.
   */
  bool hasGpsSystem() const;

  /**
   * @brief Get the GPS based coordinate system.
   *        If there is non, a unit matrix is returned.
   */
  std::shared_ptr<zfprj::CoordinateSystem> getGpsSystem();

  /**
   * @brief Return the name of the current system
   *
   * @return name of the current system
   */
  const std::wstring &getCurrentSystemName() const;

  double getProjectAngleToNorth();
  void setProjectAngleToNorth(double angle) { projectAngleToNorth = angle; }

  /**
   * @brief Load all systems from project file.
   *
   * @param globalElement the element where the global coordinate systems should
   * be added
   */
  bool load(tinyxml2::XMLElement *globalElement);

  /**
   * @brief Save to project file.
   *
   * @param globalElement the element containing the global coordinate systems
   */
  bool save(tinyxml2::XMLElement *globalElement);

  bool addMatrixToSystems(const ZFTRMatrix &distanceMatrix,
                          const std::list<std::wstring> &systems);
};
} // namespace zfprj
