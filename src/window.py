# window.py
#
# Copyright 2024 logonoff <hello@logonoff.co>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# SPDX-License-Identifier: GPL-3.0-or-later

import os
from gi.repository import Adw
from gi.repository import Gtk
from .settings_manager import SettingsManager
from .add_dialog import AddDialogBox

@Gtk.Template(resource_path='/co/logonoff/summon/window.ui')
class SummonWindow(Adw.ApplicationWindow):
    __gtype_name__ = 'SummonWindow'

    add_button: Gtk.Button = Gtk.Template.Child()
    remotes_list: Gtk.ListBox = Gtk.Template.Child()
    no_items = Gtk.Template.Child()
    wol_clients: SettingsManager

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        XDG_CONFIG_HOME = os.getenv('XDG_CONFIG_HOME', os.path.expanduser('~/.config'))

        self.wol_clients = SettingsManager(base_path=XDG_CONFIG_HOME)
        self.wol_clients.load_settings()

        self.remotes_list.get_style_context().add_class('boxed-list')

        for client in self.wol_clients.wol_clients:
            self.add_wol_client_to_list(client)

    def add_wol_client_to_list(self, wol_client):
        """Add a new WolClient to the list and update the view."""
        self.no_items.hide()

        new_row = Adw.ActionRow.new()
        new_row.set_title(wol_client.name)
        new_row.set_subtitle(wol_client.get_mac_address())
        new_row.get_style_context().add_class('AdwActionRow')

        # create start button
        start_button = Gtk.Button.new_from_icon_name('media-playback-start-symbolic')
        start_button.set_tooltip_text('Start')
        start_button.get_style_context().add_class('flat')

        new_row.add_suffix(start_button)
        start_button.connect('clicked', lambda _: wol_client.send_magic_packet())

        self.remotes_list.insert(new_row, -1)


    @Gtk.Template.Callback()
    def summon_application_add_action(self, action):
        dialog = AddDialogBox(parent=self)

        def add_button_on_click():
            new_client = dialog.generate_wol_client()
            self.wol_clients.add_wol_client(new_client)
            self.add_wol_client_to_list(new_client)
            dialog.close()

        dialog.add_button.connect('clicked', lambda _: add_button_on_click())

        dialog.present(self)
