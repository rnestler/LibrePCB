/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "packagecategoryeditorwidget.h"

#include "../common/categorychooserdialog.h"
#include "../common/categorytreelabeltextbuilder.h"
#include "ui_packagecategoryeditorwidget.h"

#include <librepcb/library/cat/packagecategory.h>
#include <librepcb/workspace/workspace.h>

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace library {
namespace editor {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

PackageCategoryEditorWidget::PackageCategoryEditorWidget(const Context& context,
                                                         const FilePath& fp,
                                                         QWidget*        parent)
  : EditorWidgetBase(context, fp, parent),
    mUi(new Ui::PackageCategoryEditorWidget) {
  mUi->setupUi(this);
  setWindowIcon(QIcon(":/img/places/folder_green.png"));
  connect(mUi->btnChooseParentCategory, &QToolButton::clicked, this,
          &PackageCategoryEditorWidget::btnChooseParentCategoryClicked);
  connect(mUi->btnResetParentCategory, &QToolButton::clicked, this,
          &PackageCategoryEditorWidget::btnResetParentCategoryClicked);
  connect(mUi->edtName, &QLineEdit::textChanged, this,
          &PackageCategoryEditorWidget::edtnameTextChanged);

  mCategory.reset(new PackageCategory(fp, false));  // can throw
  setWindowTitle(*mCategory->getNames().value(getLibLocaleOrder()));
  mUi->edtName->setText(*mCategory->getNames().value(getLibLocaleOrder()));
  mUi->edtDescription->setPlainText(
      mCategory->getDescriptions().value(getLibLocaleOrder()));
  mUi->edtKeywords->setText(
      mCategory->getKeywords().value(getLibLocaleOrder()));
  mUi->edtAuthor->setText(mCategory->getAuthor());
  mUi->edtVersion->setText(mCategory->getVersion().toStr());
  mUi->cbxDeprecated->setChecked(mCategory->isDeprecated());
  mParentUuid = mCategory->getParentUuid();
  updateCategoryLabel();

  connect(mUi->edtName, &QLineEdit::textChanged, this,
          &QWidget::setWindowTitle);
  connect(mUi->edtName, &QLineEdit::textEdited, this,
          &PackageCategoryEditorWidget::setDirty);
  connect(mUi->edtDescription, &QPlainTextEdit::textChanged, this,
          &PackageCategoryEditorWidget::setDirty);
  connect(mUi->edtKeywords, &QLineEdit::textEdited, this,
          &PackageCategoryEditorWidget::setDirty);
  connect(mUi->edtAuthor, &QLineEdit::textEdited, this,
          &PackageCategoryEditorWidget::setDirty);
  connect(mUi->edtVersion, &QLineEdit::textEdited, this,
          &PackageCategoryEditorWidget::setDirty);
  connect(mUi->cbxDeprecated, &QCheckBox::clicked, this,
          &PackageCategoryEditorWidget::setDirty);
}

PackageCategoryEditorWidget::~PackageCategoryEditorWidget() noexcept {
}

/*******************************************************************************
 *  Public Slots
 ******************************************************************************/

bool PackageCategoryEditorWidget::save() noexcept {
  try {
    ElementName name(mUi->edtName->text().trimmed());  // can throw
    Version     version =
        Version::fromString(mUi->edtVersion->text().trimmed());  // can throw
    mCategory->setName("", name);
    mCategory->setDescription("", mUi->edtDescription->toPlainText().trimmed());
    mCategory->setKeywords("", mUi->edtKeywords->text().trimmed());
    mCategory->setAuthor(mUi->edtAuthor->text().trimmed());
    mCategory->setVersion(version);
    mCategory->setParentUuid(mParentUuid);
    mCategory->setDeprecated(mUi->cbxDeprecated->isChecked());
    mCategory->save();
    return EditorWidgetBase::save();
  } catch (const Exception& e) {
    QMessageBox::critical(this, tr("Save failed"), e.getMsg());
    return false;
  }
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void PackageCategoryEditorWidget::btnChooseParentCategoryClicked() noexcept {
  PackageCategoryChooserDialog dialog(mContext.workspace);
  if (dialog.exec()) {
    mParentUuid = dialog.getSelectedCategoryUuid();
    updateCategoryLabel();
    setDirty();
  }
}

void PackageCategoryEditorWidget::btnResetParentCategoryClicked() noexcept {
  mParentUuid = tl::nullopt;
  updateCategoryLabel();
  setDirty();
}

void PackageCategoryEditorWidget::edtnameTextChanged(
    const QString& text) noexcept {
  // force updating parent categories
  Q_UNUSED(text);
  updateCategoryLabel();
}

void PackageCategoryEditorWidget::updateCategoryLabel() noexcept {
  const workspace::WorkspaceLibraryDb& db = mContext.workspace.getLibraryDb();
  PackageCategoryTreeLabelTextBuilder  textBuilder(db, getLibLocaleOrder(),
                                                  *mUi->lblParentCategories);
  textBuilder.setEndlessRecursionUuid(mCategory->getUuid());
  textBuilder.setHighlightLastLine(true);
  textBuilder.updateText(mParentUuid, mUi->edtName->text());
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace library
}  // namespace librepcb
