import inspect
import os
import yaml


class Configuration:
    def __init__(self, d=None):
        self.__dict__["_d"] = d if d is not None else dict()

    def __getattr__(self, key):
        if key[0] == '_':
            return object.__getattr__(self, key)        

        d = self.__dict__["_d"]
        if key in d:
            if isinstance(d[key], dict):
                return Configuration(d[key])
            else:
                return d[key]

        d[key] = dict()
        return Configuration(d[key])

    def __setattr__(self, key, value):
        if isinstance(value, Configuration):  
            self.__dict__["_d"][key] = value.__dict__["_d"]
        elif isinstance(value, list):
            # Replace all occurances of Configuration with the dict equivalent
            self.__dict__["_d"][key] = [ i.__dict__["_d"] if isinstance(i, Configuration) else i for i in value]
        elif isinstance(value, Configuration):
            self.__dict__["_d"][key] = value.__dict__["_d"]
        else:
            self.__dict__["_d"][key] = value

    def _dict(self):
        return self.__dict__["_d"]

    def __str__(self):
        return str(self._dict())

    def __repr__(self):
        return self._dict()


def from_file(filename, relative=True):

    if relative:
        call_filename = inspect.getouterframes(inspect.currentframe())[1][1]
        full_filename = os.path.join(os.path.dirname(call_filename), filename)
    else:
        full_filename = filename

    cfg = Configuration()

    if full_filename.endswith(".py"):
        import commands
        (stat, output) = commands.getstatusoutput("python {}".format(full_filename))
        if stat != 0:
            raise IOError(output)

        cfg.__dict__["_d"] = yaml.load(output)
    elif full_filename.endswith(".yaml") or full_filename.endswith(".yml") or full_filename.endswith(".json"):
        # since JSON is a sub-set of YAML, we can always use the YAML parser
        with open(full_filename, 'r') as f:
            cfg.__dict__["_d"] = yaml.load(f, Loader=yaml.Loader)
    else:
        raise IOError("Unknown config file format: {}".format(filename))

    return cfg


def to_yaml(config):
    return yaml.dump(config._dict(), default_flow_style=True, width=1e9)


def write_yaml(config):
    print(to_yaml(config))


