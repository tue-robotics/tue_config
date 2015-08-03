class Configuration:

    def __init__(self):
        self.__dict__["_values"] = {}
        self.__dict__["_groups"] = {}
        self.__dict__["_array"] = None
        self.__dict__["_dict"] = {}

    def __iadd__(self, value):
        a = self.__dict__["_array"]
        if not a:
            a = []
        a += value
        self.__dict__["_array"] = a
        return self

    def __getattr__(self, key):
        if key[0] == '_':
            return object.__getattr__(self, key)

        if key in self.__dict__["_values"]:
            raise AttributeError, "Cannot create group '{}': is already a value name".format(key)

        groups = self.__dict__["_groups"]
        if not key in groups:
            groups[key] = Configuration()
        return groups[key]

    def __setattr__(self, name, value):
        if isinstance(value, Configuration):
            if name in self.__dict__["_values"]:
                raise AttributeError, "Cannot create group '{}': is already a value name".format(name)
            self.__dict__["_groups"][name] = value
            return
        elif isinstance(value, list):
            if value and isinstance(value[0], Configuration):
                g = self.__getattr__(name)
                g.__dict__["_array"] = value
                return

        if name in self.__dict__["_groups"]:
            raise AttributeError, "Cannot assign '{}': is already a group name".format(name)
        self.__dict__["_values"][name] = value

    def _toDict(self):
        d = self.__dict__["_dict"]

        a = self.__dict__["_array"]
        if a:
            return [i._toDict() for i in a]

        d.update(self.__dict__["_values"].copy())

        for (k, g) in self.__dict__["_groups"].iteritems():
            d[k] = g._toDict()

        return d

# ----------------------------------------------------------------------------------------------------

#def from_dict(d):
#    print "BLAA: {}".format(d)
#
#    cfg = Configuration()
#    for (k, v) in d.iteritems():
#        if isinstance(v, dict):
#            cfg.__setattr__(k, from_dict(v))
#        elif isinstance(v, list):
#            if v and isinstance(v[0], dict):
#                for i in v:
#                    g = cfg.__getattr__(k)     
#                    #cfg.__dict__["_array"] += [from_dict(i)]    
#                    g.__iadd__([from_dict(i)])
#            else:
#                cfg.__setattr__(k, v)
#        else:
#            cfg.__setattr__(k, v)
#
#    return cfg

# ----------------------------------------------------------------------------------------------------

def from_file(filename, relative=True):

    if relative:
        import inspect, os
        call_filename = inspect.getouterframes(inspect.currentframe())[1][1]
        full_filename = os.path.join(os.path.dirname(call_filename), filename)
    else:
        full_filename = filename

    cfg = Configuration()

    if full_filename.endswith(".py"):
        import commands
        (stat, output) = commands.getstatusoutput("python {}".format(full_filename))
        if stat != 0:
            raise AttributeError, output

        import yaml
        cfg.__dict__["_dict"] = yaml.load(output)
    elif full_filename.endswith(".yaml") or full_filename.endswith(".yml") or full_filename.endswith(".json"):
        # since JSON is a sub-set of YAML, we can always use the YAML parser
        import yaml
        with open(full_filename, 'r') as f:
            cfg.__dict__["_dict"] = yaml.load(f)
    else:
        raise AttributeError, "Unknown config file format: {}".format(filename)

    return cfg

# ----------------------------------------------------------------------------------------------------

def write_yaml(config):
    import yaml
    print yaml.dump(config._toDict(), default_flow_style=True, width=1e9)



