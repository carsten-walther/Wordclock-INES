import { Component, Element, h, Prop, State } from '@stencil/core';
import { alertController } from "@ionic/core";
import { AjaxService } from "../../services/ajax-service";
import { ModeService } from "../../services/mode-service";
import { TimezoneService } from "../../services/timezone-service";
import { LanguageService } from "../../services/language-service";
import { FavoriteService } from "../../services/favorite-service";

@Component({
  tag: 'app-home',
  styleUrl: 'app-home.scss'
})
export class AppHome {

  @Element() element: HTMLElement;

  @State() settings: Settings = {};
  @State() modes: Mode[] = [];
  @State() timezones: Timezone[] = [];
  @State() languages: Language[] = [];
  @State() favorites: Favorite[] = [];

  @Prop() isDisabled: boolean = true;

  async componentWillLoad() {
    let settingsData = await AjaxService.getSettings();
    this.settings = {...settingsData.result};
    this.modes = [...await ModeService.get()];
    this.timezones = [...await TimezoneService.get()];
    this.languages = [...await LanguageService.get()];
    this.favorites = [...await FavoriteService.get()];

    if (settingsData.success) {
      this.isDisabled = false;
    }
    this.isDisabled = false;
  }

  async addFavorite(foregroundColor: Color, backgroundColor: Color, brightness: number) {
    const alert = await alertController.create({
      header: 'Add Favorite',
      message: 'Please enter title',
      inputs: [{
        name: 'title',
        placeholder: 'My favorite'
      }],
      buttons: [{
        text: 'Cancel',
        role: 'cancel'
      }, {
        text: 'Ok',
        handler: async data => {
          await FavoriteService.add({
            title: data.title,
            foregroundColor: foregroundColor,
            backgroundColor: backgroundColor,
            brightness: brightness
          });
          this.favorites = [...await FavoriteService.get()]
        }
      }]
    });
    await alert.present();
  }

  async removeFavorite(favorite: Favorite) {
    const ionListElement: HTMLIonListElement = this.element.querySelector('ion-list');
    let itemListCloseState = await ionListElement.closeSlidingItems();
    if (itemListCloseState) {
      this.favorites.splice(this.favorites.indexOf(favorite), 1);
      this.favorites = [...this.favorites];
      await FavoriteService.remove(favorite);
    }
  }

  setFavorite(favorite: Favorite) {
    this.settings = {...this.settings, foregroundColor: favorite.foregroundColor};
    this.settings = {...this.settings, backgroundColor: favorite.backgroundColor};
    this.settings = {...this.settings, brightness: favorite.brightness};

    this.updateSettings('foregroundColor', this.settings.foregroundColor);
    this.updateSettings('backgroundColor', this.settings.backgroundColor);
    this.updateSettings('brightness', this.settings.brightness);
  }

  checkFavorite(favorite: Favorite) {
    if (this.settings.foregroundColor == favorite.foregroundColor && this.settings.backgroundColor == favorite.backgroundColor && this.settings.brightness == favorite.brightness) {
      return <ion-icon name="checkmark" slot="end"/>;
    } else {
      return null;
    }
  }

  async updateSettings(field: string, value: any) {
    this.settings = {...this.settings, [field]: value};
    await AjaxService.setSettings(field, this.settings[field]);
  }

  renderModes() {
    return this.modes.map(mode => {
      return (
        <ion-select-option value={mode.value}>{mode.label}</ion-select-option>
      )
    });
  }

  renderTimezones() {
    return this.timezones.map(timezone => {
      return (
        <ion-select-option value={timezone.value}>{timezone.label}</ion-select-option>
      )
    });
  }

  renderLanguages() {
    return this.languages.map(language => {
      return (
        <ion-select-option value={language.value}>{language.label}</ion-select-option>
      )
    });
  }

  renderFavorites() {
    return this.favorites.map((favorite: Favorite) => {
      let options = null;
      if (!favorite.locked) {
        options = (
          <ion-item-options side="end">
            <ion-item-option color="danger" expandable={true} onClick={() => this.removeFavorite(favorite)}>
              <ion-icon slot="start" name="trash" size="large"/>
              Remove
            </ion-item-option>
          </ion-item-options>
        );
      }
      let locked = null;
      if (favorite.locked) {
        locked = (
          <ion-icon name="lock-closed" size="small" slot="end" color="medium"/>
        );
      }
      return (
        <ion-item-sliding>
          <ion-item onClick={() => this.setFavorite(favorite)} disabled={this.isDisabled}>
            <div slot="start" class="color-avatar" style={{backgroundImage: `linear-gradient(to bottom right, rgb(${favorite.foregroundColor.red}, ${favorite.foregroundColor.green}, ${favorite.foregroundColor.blue}), rgb(${favorite.backgroundColor.red}, ${favorite.backgroundColor.green}, ${favorite.backgroundColor.blue}))`}}>&nbsp;</div>
            <ion-label>
              {favorite.title}
            </ion-label>
            {this.checkFavorite(favorite)}
            {locked}
          </ion-item>
          {options}
        </ion-item-sliding>
      )
    });
  }

  renderFavoriteButton() {
    if (!this.isDisabled) {
      return (
        <ion-chip slot="end" onClick={() => this.addFavorite(this.settings.foregroundColor, this.settings.backgroundColor, this.settings.brightness)}>
          <ion-label>Favorite</ion-label>
          <ion-icon name="heart" color="danger"/>
        </ion-chip>
      );
    }
  }

  render() {
    return [
      <ion-header translucent={true}>
        <ion-toolbar>
          <ion-title>Wordclock</ion-title>
        </ion-toolbar>
      </ion-header>,

      <ion-content>
        <ion-list>

          <ion-item-group>
            <ion-item-divider color="light" sticky={true}>
              <ion-label>Colors</ion-label>
            </ion-item-divider>
            <ion-item disabled={this.isDisabled}>
              <ion-label>Foreground</ion-label>
              <ion-input type="text" value="333" />
            </ion-item>
            <ion-item disabled={this.isDisabled}>
              <ion-label>Background</ion-label>
              <ion-input type="text" value="333" />
            </ion-item>
            <ion-item disabled={this.isDisabled}>
              <ion-range min={1} max={100} value={this.settings.brightness} debounce={500} onIonChange={ev => this.updateSettings('brightness', ev.detail.value)}>
                <ion-icon slot="start" size="small" name="sunny"/>
                <ion-icon slot="end" name="sunny"/>
              </ion-range>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item-divider color="light" sticky={true}>
              <ion-label>Modes</ion-label>
            </ion-item-divider>
            <ion-item disabled={this.isDisabled}>
              <ion-label>Mode</ion-label>
              <ion-select value={this.settings.clockMode} onIonChange={ev => this.updateSettings('clockMode', ev.detail.value)}>
                {this.renderModes()}
              </ion-select>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item-divider color="light" sticky={true}>
              <ion-label>Settings</ion-label>
            </ion-item-divider>
            <ion-item disabled={this.isDisabled}>
              <ion-label>Sleep time</ion-label>
              <ion-datetime displayFormat="HH:mm" value={`${this.settings.sleepHour}:${this.settings.sleepMinute}`} onIonChange={ev => this.updateSettings('sleepTime', ev.detail.value)}/>
            </ion-item>
            <ion-item disabled={this.isDisabled}>
              <ion-label>Wakeup time</ion-label>
              <ion-datetime displayFormat="HH:mm" value={`${this.settings.wakeupHour}:${this.settings.wakeupMinute}`} onIonChange={ev => this.updateSettings('wakeupTime', ev.detail.value)}/>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item disabled={this.isDisabled}>
              <ion-label>Timezone</ion-label>
              <ion-select value={this.settings.timeZone} onIonChange={ev => this.updateSettings('timeZone', ev.detail.value)}>
                {this.renderTimezones()}
              </ion-select>
            </ion-item>
            <ion-item disabled={this.isDisabled}>
              <ion-label>Daylight savings time</ion-label>
              <ion-toggle checked={this.settings.daylightSavingsTime} onIonChange={ev => this.updateSettings('daylightSavingsTime', ev.detail.checked)}/>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item disabled={this.isDisabled}>
              <ion-label>Language</ion-label>
              <ion-select value={this.settings.language} onIonChange={ev => this.updateSettings('language', ev.detail.value)}>
                {this.renderLanguages()}
              </ion-select>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item-divider color="light" sticky={true}>
              <ion-label>Favorites</ion-label>
              {this.renderFavoriteButton()}
            </ion-item-divider>
            {this.renderFavorites()}
          </ion-item-group>

        </ion-list>
      </ion-content>
    ];
  }
}
