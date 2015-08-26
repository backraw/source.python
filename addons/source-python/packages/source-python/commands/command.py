# ../commands/command.py

"""Contains a base decorator class for registering commands."""

# =============================================================================
# >> IMPORTS
# =============================================================================
# Source.Python Imports
#   Core
from core import AutoUnload


# =============================================================================
# >> CLASSES
# =============================================================================
class _BaseCommand(AutoUnload):

    """Base decorator class used to register commands."""

    def __init__(self, names, *args, **kwargs):
        """Store the base values for the decorator."""
        self.names = names
        self.args = args
        self.kwargs = kwargs
        self.callback = None

    def __call__(self, callback):
        """Register the commands to the given callback."""
        # Is there another registered decorator for the callback?
        if isinstance(callback, _BaseCommand):

            # Store the callback
            self.callback = callback.callback

        # Is this the first decorator registering the callback?
        else:

            # Store the callback
            self.callback = callback

        # Register the commands
        self._manager_class.register_commands(
            self.names, self.callback, *self.args, **self.kwargs)

        # Return the object
        return self

    def _unload_instance(self):
        """Unregister the commands."""
        self._manager_class.unregister_commands(self.names, self.callback)
