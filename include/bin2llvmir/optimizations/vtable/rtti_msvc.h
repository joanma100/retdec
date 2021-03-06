/**
 * @file include/bin2llvmir/optimizations/vtable/rtti_msvc.h
 * @brief Search for msvc RTTI in input file.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 *
 * http://www.openrce.org/articles/full_view/21
 * http://www.openrce.org/articles/full_view/23
 */

#ifndef BIN2LLVMIR_OPTIMIZATIONS_VTABLE_RTTI_MSVC_H
#define BIN2LLVMIR_OPTIMIZATIONS_VTABLE_RTTI_MSVC_H

#include <cassert>
#include <cstdint>
#include <vector>

#include "tl-cpputils/address.h"

namespace bin2llvmir {

class RTTICompleteObjectLocator;
class RTTITypeDescriptor;
class RTTIClassHierarchyDescriptor;
class RTTIBaseClassDescriptor;

/**
 * Describes a single C++ type
 */
class RTTITypeDescriptor
{
	// ABI specification.
	//
	public:
		/// Virtual table of @c type_info class.
		tl_cpputils::Address vtableAddr;
		/// Used to keep the demangled name returned by type_info::name()
		tl_cpputils::Address spare;
		/// Mangled type name, e.g. ".H" = "int", ".?AVA@@" = "class A".
		std::string name;

	// Our methods and data.
	//
	public:
		std::string dump() const;

	public:
		/// Position of this @c RTTITypeDescriptor entry.
		tl_cpputils::Address address;
		/// Virtual table object on address @c vtableAddr.
		/// ...
};

/**
 * Describes base class together with information which allows compiler
 * to cast the derived class to it during execution of the _dynamic_cast_.
 */
class RTTIBaseClassDescriptor
{
	// ABI specification.
	//
	public:
		struct PMD
		{
			/// Member displacement.
			int32_t mdisp = 0;
			/// Vbtable displacement
			int32_t pdisp = 0;
			/// Displacement inside vbtable.
			int32_t vdisp = 0;
		};

	public:
		/// Type descriptor of the class.
		tl_cpputils::Address typeDescriptorAddr;
		/// Number of nested classes following in the Base Class Array.
		uint32_t numContainedBases = 0;
		/// Pointer-to-member displacement info.
		PMD where;
		/// Flags, usually 0.
		uint32_t attributes = 0;

	// Our methods and data.
	//
	public:
		std::string dump() const;

	public:
		/// Position of this @c RTTIBaseClassDescriptor entry.
		tl_cpputils::Address address;
		/// RTTITypeDescriptor object on address @c typeDescriptorAddr.
		RTTITypeDescriptor* typeDescriptor = nullptr;
};

/**
 * Descriptor describes the inheritance hierarchy of the class.
 * It is shared by all COLs for a class.
 */
class RTTIClassHierarchyDescriptor
{
	// ABI specification.
	//
	public:
		/// Always zero?
		uint32_t signature = 0;
		/// Bit 0 set = multiple inheritance, bit 1 set = virtual inheritance.
		uint32_t attributes = 0;
		/// Number of classes in pBaseClassArray.
		uint32_t numBaseClasses = 0;
		/// Address of base class objects array.
		tl_cpputils::Address baseClassArrayAddr;

	// Our methods and data.
	//
	public:
		std::string dump() const;

	public:
		/// Position of this @c RTTIClassHierarchyDescriptor entry.
		tl_cpputils::Address address;
		/// Array of addresses of base class objects on address
		/// @c baseClassArrayAddr.
		std::vector<tl_cpputils::Address> baseClassArray;
		/// Base class objects - on addresses from @c baseClassArray.
		/// Together contain information which allows compiler to cast the
		/// derived class to any of them during execution of the _dynamic_cast_.
		std::vector<RTTIBaseClassDescriptor*> baseClasses;
};

/**
 * Compiler puts pointer to this structure just before vftable.
 * This structure allows to find the locations of the complete
 * object from a specific vftable pointer.
 */
class RTTICompleteObjectLocator
{
	// ABI specification.
	//
	public:
		/// Always zero?
		uint32_t signature = 0;
		/// Offset of this vtable in the complete class.
		uint32_t offset = 0;
		/// Constructor displacement offset.
		uint32_t cdOffset = 0;
		/// Pointer (address) of type descriptor for this object.
		tl_cpputils::Address typeDescriptorAddr;
		/// Pointer (address) of class descriptor for this object.
		tl_cpputils::Address classDescriptorAddr;

	// Our methods and data.
	//
	public:
		std::string dump() const;

	public:
		/// Position of this @c RTTICompleteObjectLocator entry.
		tl_cpputils::Address address;
		/// RTTITypeDescriptor object on address @c typeDescriptorAddr.
		RTTITypeDescriptor* typeDescriptor = nullptr;
		/// RTTIClassHierarchyDescriptor object on address
		/// @c classDescriptorAddr.
		RTTIClassHierarchyDescriptor* classDescriptor = nullptr;
};

} // namespace bin2llvmir

#endif
