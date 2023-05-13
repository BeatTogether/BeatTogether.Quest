#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/Installer.hpp"

DECLARE_CLASS_CODEGEN(BeatTogether::Installers, MenuInstaller, Zenject::Installer,
    DECLARE_OVERRIDE_METHOD(void, InstallBindings, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Zenject::Installer::InstallBindings>::get());

    DECLARE_DEFAULT_CTOR();
)
