/* summon-application.c
 *
 * Copyright 2024 logonoff <hello@logonoff.co>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config.h"

#include "summon-application.h"
#include "summon-window.h"

struct _SummonApplication
{
	AdwApplication parent_instance;
};

G_DEFINE_FINAL_TYPE (SummonApplication, summon_application, ADW_TYPE_APPLICATION)

SummonApplication *
summon_application_new (const char        *application_id,
                        GApplicationFlags  flags)
{
	g_return_val_if_fail (application_id != NULL, NULL);

	return g_object_new (SUMMON_TYPE_APPLICATION,
	                     "application-id", application_id,
	                     "flags", flags,
	                     NULL);
}

static void
summon_application_activate (GApplication *app)
{
	GtkWindow *window;

	g_assert (SUMMON_IS_APPLICATION (app));

	window = gtk_application_get_active_window (GTK_APPLICATION (app));

	if (window == NULL)
		window = g_object_new (SUMMON_TYPE_WINDOW,
		                       "application", app,
		                       NULL);

	gtk_window_present (window);
}

static void
summon_application_class_init (SummonApplicationClass *klass)
{
	GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

	app_class->activate = summon_application_activate;
}

static void
summon_application_about_action (GSimpleAction *action,
                                 GVariant      *parameter,
                                 gpointer       user_data)
{
	// static const char *developers[] = {"logonoff", NULL};
	SummonApplication *self = user_data;
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (self));
	AdwDialog *about;

	g_assert (SUMMON_IS_APPLICATION (self));

    about = adw_about_dialog_new_from_appdata (
		"/co/logonoff/summon/metainfo",
        PACKAGE_VERSION
	);

	adw_about_dialog_set_version (ADW_ABOUT_DIALOG (about), PACKAGE_VERSION);

	adw_dialog_present (ADW_DIALOG (about), GTK_WIDGET (window));
}

static void
summon_application_quit_action (GSimpleAction *action,
                                GVariant      *parameter,
                                gpointer       user_data)
{
	SummonApplication *self = user_data;

	g_assert (SUMMON_IS_APPLICATION (self));

	g_application_quit (G_APPLICATION (self));
}

static const GActionEntry app_actions[] = {
	{ "quit", summon_application_quit_action },
	{ "about", summon_application_about_action },
};

static void
summon_application_init (SummonApplication *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self),
	                                 app_actions,
	                                 G_N_ELEMENTS (app_actions),
	                                 self);
	gtk_application_set_accels_for_action (GTK_APPLICATION (self),
	                                       "app.quit",
	                                       (const char *[]) { "<primary>q", NULL });
}
