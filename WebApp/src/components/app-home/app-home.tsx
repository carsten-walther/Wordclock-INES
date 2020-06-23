import { Component, Element, h, Prop, State } from '@stencil/core';
import { alertController, modalController, toastController } from "@ionic/core";
import { AjaxService } from "../../services/ajax-service";
import { DefaultsService } from "../../services/defaults-service";
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
    this.settings = {...await DefaultsService.get()};
    this.modes = [...await ModeService.get()];
    this.timezones = [...await TimezoneService.get()];
    this.languages = [...await LanguageService.get()];
    this.favorites = [...await FavoriteService.get()];

    this.isDisabled = true;
  }

  async componentDidLoad() {
    let settingsData = await AjaxService.getSettings();

    if (settingsData != null) {
      if (settingsData.success) {
        this.isDisabled = false;
        this.settings = {...settingsData.result};
      }
    } else {
      const toast = await toastController.create({
        header: 'Connection Error',
        message: 'Word clock is not available.<br>Please check the network connection.',
        buttons: [{
          text: 'Hm, okay...',
          role: 'cancel'
        }],
        color: 'danger'
      });
      await toast.present();
      await toast.onWillDismiss();
    }
  }

  calcTime(hour: number, minute: number) {
    let hourString = hour < 10 ? '0' + hour : hour;
    let minuteString = minute < 10 ? '0' + minute : minute;
    return `${hourString}:${minuteString}`;
  }

  isFavorite(favorite: Favorite) {
    if (this.settings.foregroundColor == favorite.foregroundColor && this.settings.backgroundColor == favorite.backgroundColor && this.settings.brightness == favorite.brightness) {
      return true
    } else {
      return null;
    }
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
        text: 'Nope',
        role: 'cancel'
      }, {
        text: 'Yay',
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

  async setFavorite(favorite: Favorite) {
    this.settings = {...this.settings, foregroundColor: favorite.foregroundColor};
    this.settings = {...this.settings, backgroundColor: favorite.backgroundColor};
    this.settings = {...this.settings, brightness: favorite.brightness};

    this.updateSettings('foregroundColor', this.settings.foregroundColor);
    this.updateSettings('backgroundColor', this.settings.backgroundColor);
    this.updateSettings('brightness', this.settings.brightness);
  }

  async updateSettings(field: string, value: any) {
    this.settings = {...this.settings, [field]: value};
    await AjaxService.setSettings(field, this.settings[field]);
  }

  async presentColorPicker(what: string, color: Color, title: string) {
    const modal = await modalController.create({
      component: 'app-color-picker',
      swipeToClose: true,
      presentingElement: this.element.closest('ion-router-outlet'),
      componentProps: {
        header: title,
        color: color
      }
    });

    await modal.present();

    let data = await modal.onWillDismiss();
    this.settings[what] = data.data;
    await this.updateSettings(what, this.settings[what]);
  }

  async presentAbout() {
    const modal = await modalController.create({
      component: 'app-about',
      swipeToClose: true,
      presentingElement: this.element.closest('ion-router-outlet')
    });

    await modal.present();
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
          <ion-item  button={true} onClick={() => this.setFavorite(favorite)} disabled={this.isDisabled}>
            <div slot="start" class="color-avatar" style={{backgroundImage: `linear-gradient(to bottom right, rgb(${favorite.foregroundColor.red}, ${favorite.foregroundColor.green}, ${favorite.foregroundColor.blue}), rgb(${favorite.backgroundColor.red}, ${favorite.backgroundColor.green}, ${favorite.backgroundColor.blue}))`}}> </div>
            <ion-label>{favorite.title}</ion-label>
            {this.isFavorite(favorite) && <ion-icon name="checkmark" slot="end"/>}
            {locked}
          </ion-item>
          {options}
        </ion-item-sliding>
      )
    });
  }

  render() {
    return [
      <ion-header>
        <ion-toolbar>
          <ion-title>Wordclock</ion-title>
        </ion-toolbar>
      </ion-header>,

      <ion-content fullscreen={true}>
        <ion-header collapse="condense">
          <ion-toolbar>
            <ion-title size="large">Wordclock</ion-title>
          </ion-toolbar>
        </ion-header>

        <ion-list lines="full" class="ion-no-padding">
          <ion-item-group>
            <ion-item-divider color="light" sticky={true}>
              <h3 class="ion-padding-top">Colors</h3>
            </ion-item-divider>
            <ion-item disabled={this.isDisabled} button={true} onClick={() => this.presentColorPicker('foregroundColor', this.settings.foregroundColor, 'Foreground color')}>
              <ion-icon slot="start" class="color-avatar" name="radio-button-on" style={{color: `rgb(${this.settings.foregroundColor.red}, ${this.settings.foregroundColor.green}, ${this.settings.foregroundColor.blue})`, backgroundColor: `rgb(${this.settings.foregroundColor.red}, ${this.settings.foregroundColor.green}, ${this.settings.foregroundColor.blue})`, borderRadius: '50%'}}/>
              <ion-label>Foreground</ion-label>
              <ion-icon slot="end" name="options"/>
            </ion-item>
            <ion-item disabled={this.isDisabled} button={true} onClick={() => this.presentColorPicker('backgroundColor', this.settings.backgroundColor, 'Background color')}>
              <ion-icon slot="start" class="color-avatar" name="radio-button-on" style={{color: `rgb(${this.settings.backgroundColor.red}, ${this.settings.backgroundColor.green}, ${this.settings.backgroundColor.blue})`, backgroundColor: `rgb(${this.settings.backgroundColor.red}, ${this.settings.backgroundColor.green}, ${this.settings.backgroundColor.blue})`, borderRadius: '50%'}}/>
              <ion-label>Background</ion-label>
              <ion-icon slot="end" name="options"/>
            </ion-item>
            <ion-item disabled={this.isDisabled}>
              <ion-icon slot="start" name="contrast-outline"/>
              <ion-range min={1} max={100} pin={true} value={this.settings.brightness} debounce={500} onIonChange={ev => this.updateSettings('brightness', ev.detail.value)}>
                <ion-icon slot="start" size="small" name="sunny"/>
                <ion-icon slot="end" name="sunny"/>
              </ion-range>
            </ion-item>
            <ion-item disabled={this.isDisabled} button={true} onClick={() => this.addFavorite(this.settings.foregroundColor, this.settings.backgroundColor, this.settings.brightness)}>
              <ion-icon slot="start" name="heart-outline"/>
              <ion-label>Add to favorites</ion-label>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item-divider color="light" sticky={true}>
              <h3 class="ion-padding-top">Favorites</h3>
            </ion-item-divider>
            {this.renderFavorites()}
          </ion-item-group>

          <ion-item-group>
            <ion-item-divider color="light" sticky={true}>
              <h3 class="ion-padding-top">Modes</h3>
            </ion-item-divider>
            <ion-item disabled={this.isDisabled}>
              <ion-icon slot="start" name="chatbubble-ellipses-outline"/>
              <ion-label>Mode</ion-label>
              <ion-select value={this.settings.clockMode} onIonChange={ev => this.updateSettings('clockMode', ev.detail.value)}>
                {this.modes.map(mode => {
                  return (
                    <ion-select-option value={mode.value}>{mode.label}</ion-select-option>
                  )
                })}
              </ion-select>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item-divider color="light" sticky={true}>
              <h3 class="ion-padding-top">Settings</h3>
            </ion-item-divider>
            <ion-item disabled={this.isDisabled}>
              <ion-icon slot="start" name="alarm-outline"/>
              <ion-label>Sleep time</ion-label>
              <ion-datetime displayFormat="HH:mm" value={this.calcTime(this.settings.sleepHour, this.settings.sleepMinute)} onIonChange={ev => this.updateSettings('sleepTime', ev.detail.value)}/>
            </ion-item>
            <ion-item disabled={this.isDisabled}>
              <ion-icon slot="start" name="alarm-outline"/>
              <ion-label>Wakeup time</ion-label>
              <ion-datetime displayFormat="HH:mm" value={this.calcTime(this.settings.wakeupHour, this.settings.wakeupMinute)} onIonChange={ev => this.updateSettings('wakeupTime', ev.detail.value)}/>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item disabled={this.isDisabled}>
              <ion-icon slot="start" name="timer-outline"/>
              <ion-label>Timezone</ion-label>
              <ion-select value={this.settings.timeZone} onIonChange={ev => this.updateSettings('timeZone', ev.detail.value)}>
                {this.timezones.map(timezone => {
                  return (
                    <ion-select-option value={timezone.value}>{timezone.label}</ion-select-option>
                  )
                })}
              </ion-select>
            </ion-item>
            <ion-item disabled={this.isDisabled}>
              <ion-icon slot="start" name="flashlight-outline"/>
              <ion-label>Daylight savings time</ion-label>
              <ion-toggle checked={this.settings.daylightSavingsTime} onIonChange={ev => this.updateSettings('daylightSavingsTime', ev.detail.checked)}/>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item disabled={this.isDisabled}>
              <ion-icon slot="start" name="language-outline"/>
              <ion-label>Language</ion-label>
              <ion-select value={this.settings.language} onIonChange={ev => this.updateSettings('language', ev.detail.value)}>
                {this.languages.map(language => {
                  return (
                    <ion-select-option value={language.value}>{language.label}</ion-select-option>
                  )
                })}
              </ion-select>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item-divider color="light"> </ion-item-divider>
            <ion-item button={true} onClick={() => this.presentAbout()}>
              <ion-icon slot="start" name="information-circle-outline"/>
              <ion-label>About</ion-label>
            </ion-item>
          </ion-item-group>

          <ion-item-group>
            <ion-item-divider color="light"> </ion-item-divider>
          </ion-item-group>

        </ion-list>
      </ion-content>
    ];
  }
}
