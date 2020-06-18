import { Component, Element, State, Prop, h } from '@stencil/core';
import { alertController } from "@ionic/core";
import { AjaxService } from "../../services/ajax-service";
import { FavoriteService } from "../../services/favorite-service";

@Component({
  tag: 'app-home',
  styleUrl: 'app-home.scss'
})
export class AppHome {

  @Element() element: HTMLElement;

  @State() settings: Settings;
  @State() favorites: Favorite[];

  @Prop() isDisabled: boolean = true;

  async componentWillLoad() {
      this.favorites = [];
      this.favorites = [...await FavoriteService.get()];
      this.settings = {...await AjaxService.getSettings()};

      if (this.settings.time) {
        this.isDisabled = false;
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

      <ion-content class="ion-padding">
        <ion-list>
          <ion-item-group>
            <ion-item-divider>
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
