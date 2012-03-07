/* This file is part of Clementine.
   Copyright 2012, David Sansome <me@davidsansome.com>
   
   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "addpodcastbyurl.h"
#include "podcastdiscoverymodel.h"
#include "podcasturlloader.h"
#include "ui_addpodcastbyurl.h"
#include "core/closure.h"

#include <QNetworkReply>
#include <QMessageBox>

AddPodcastByUrl::AddPodcastByUrl(Application* app, QWidget* parent)
  : AddPodcastPage(app, parent),
    ui_(new Ui_AddPodcastByUrl),
    loader_(new PodcastUrlLoader(this))
{
  ui_->setupUi(this);
  connect(ui_->go, SIGNAL(clicked()), SLOT(GoClicked()));
}

AddPodcastByUrl::~AddPodcastByUrl() {
  delete ui_;
}

void AddPodcastByUrl::GoClicked() {
  emit Busy(true);
  model()->clear();

  PodcastUrlLoaderReply* reply = loader_->Load(ui_->url->text());
  ui_->url->setText(reply->url().toString());

  NewClosure(reply, SIGNAL(Finished(bool)),
             this, SLOT(RequestFinished(PodcastUrlLoaderReply*)),
             reply);
}

void AddPodcastByUrl::RequestFinished(PodcastUrlLoaderReply* reply) {
  reply->deleteLater();

  emit Busy(false);

  if (!reply->is_success()) {
    QMessageBox::warning(this, tr("Failed to load podcast"),
                         reply->error_text(), QMessageBox::Close);
    return;
  }

  foreach (const Podcast& podcast, reply->results()) {
    model()->appendRow(model()->CreatePodcastItem(podcast));
  }
}