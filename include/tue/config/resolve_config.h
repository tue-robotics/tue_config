#ifndef TUE_CONFIG_RESOLVE_CONFIG_H_
#define TUE_CONFIG_RESOLVE_CONFIG_H_

namespace tue
{

namespace config
{

/**
 * @brief Class to config the resolve behaviour of a loader.
 *
 * As the resolve function can be used recursively, the user should always consider whether the choices made will result in the desired behaviour.
 * Outer resolve functions can't be resolved, when inner resolve functions are skipped.
 */
class ResolveConfig
{
public:

    ResolveConfig(bool env_=true, bool file_=true, bool rospkg_=true) : env(env_), file(file_), rospkg(rospkg_) {}

    /**
     * @brief Default ResolveConfig that has all resolve options enabled
     * @return ResolveConfig with everything enabled.
     */
    static ResolveConfig defaultConfig() { return ResolveConfig(true, true, true); }

    // Resolve environment variables
    bool env;
    // Resolve filepaths
    bool file;
    // Resolve ros package names to paths
    bool rospkg;

    inline bool AllTrue() const { return env && file && rospkg; }

    inline bool AllFalse() const { return !env && !file && !rospkg; }

};

}

}

#endif
