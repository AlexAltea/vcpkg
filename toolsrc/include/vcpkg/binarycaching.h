#pragma once

#include <vcpkg/base/expected.h>
#include <vcpkg/base/files.h>
#include <vcpkg/packagespec.h>
#include <vcpkg/vcpkgpaths.h>

namespace vcpkg::Dependencies
{
    struct InstallPlanAction;
    struct ActionPlan;
}
namespace vcpkg::Build
{
    struct AbiTagAndFile;
    struct BuildPackageOptions;
}

namespace vcpkg
{
    enum class RestoreResult
    {
        missing,
        success,
        build_failed,
    };

    struct IBinaryProvider
    {
        virtual ~IBinaryProvider() = default;
        /// Gives the BinaryProvider an opportunity to batch any downloading or server communication for executing
        /// `plan`.
        virtual void prefetch(const VcpkgPaths& paths, const Dependencies::ActionPlan& plan) = 0;
        /// Attempts to restore the package referenced by `action` into the packages directory.
        virtual RestoreResult try_restore(const VcpkgPaths& paths, const Dependencies::InstallPlanAction& action) = 0;
        /// Called upon a successful build of `action`
        virtual void push_success(const VcpkgPaths& paths, const Dependencies::InstallPlanAction& action) = 0;
        /// Called upon a failure during the build of `action`
        virtual void push_failure(const VcpkgPaths& paths, const std::string& abi_tag, const PackageSpec& spec) = 0;
        /// Requests the result of `try_restore()` without actually downloading the package. Used by CI to determine
        /// missing packages.
        virtual RestoreResult precheck(const VcpkgPaths& paths,
                                       const Dependencies::InstallPlanAction& action) = 0;
    };

    IBinaryProvider& null_binary_provider();

    ExpectedS<std::unique_ptr<IBinaryProvider>> create_binary_provider_from_configs(const VcpkgPaths& paths,
                                                                                    View<std::string> args);
    ExpectedS<std::unique_ptr<IBinaryProvider>> create_binary_provider_from_configs_pure(const std::string& env_string,
                                                                                         View<std::string> args);

    void help_topic_binary_caching(const VcpkgPaths& paths);
}
