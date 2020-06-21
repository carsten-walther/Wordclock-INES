import { Component, Element, Prop, h } from '@stencil/core';

@Component({
  tag: 'app-color-picker',
  styleUrl: 'app-color-picker.scss'
})
export class AppColorPicker {

  @Element() element: HTMLElement;

  @Prop() title: string;
  @Prop() color: Color;

  updateColor(field: string, value: any) {
    this.color = {...this.color, [field]: value};
  }

  dismiss() {
    (this.element.closest('ion-modal') as any).dismiss(this.color);
  }

  apply() {
    this.dismiss();
  }

  render() {
    return [
      <ion-header>
        <ion-toolbar>
          <ion-buttons slot="start">
            <ion-button onClick={() => this.dismiss()}>Nope</ion-button>
          </ion-buttons>
          <ion-title>{this.title}</ion-title>
          <ion-buttons slot="end">
            <ion-button onClick={() => this.apply()} strong={true}>Yay</ion-button>
          </ion-buttons>
        </ion-toolbar>
      </ion-header>,

      <ion-content fullscreen={false} scrollX={false} scrollY={false}>
        <ion-grid class="ion-no-padding">
          <ion-row>
            <ion-col size="12" sizeSm="6">
              <div class="color-display" id="color-display" style={{backgroundColor: `rgb(${this.color.red},${this.color.green},${this.color.blue})`}}/>
            </ion-col>
            <ion-col size="12" sizeSm="6">
              <ion-list>
                <ion-item-group>
                  <ion-item>
                    <ion-labe>R</ion-labe>
                    <ion-range min={1} max={255} pin value={this.color.red} onIonChange={event => this.updateColor('red', event.detail.value)}>
                      <ion-icon slot="start" size="small" name="brush"/>
                      <ion-icon slot="end" name="brush"/>
                    </ion-range>
                  </ion-item>
                  <ion-item>
                    <ion-labe>G</ion-labe>
                    <ion-range min={1} max={255} pin value={this.color.green} onIonChange={event => this.updateColor('green', event.detail.value)}>
                      <ion-icon slot="start" size="small" name="brush"/>
                      <ion-icon slot="end" name="brush"/>
                    </ion-range>
                  </ion-item>
                  <ion-item>
                    <ion-labe>B</ion-labe>
                    <ion-range min={1} max={255} pin value={this.color.blue} onIonChange={event => this.updateColor('blue', event.detail.value)}>
                      <ion-icon slot="start" size="small" name="brush"/>
                      <ion-icon slot="end" name="brush"/>
                    </ion-range>
                  </ion-item>
                </ion-item-group>
              </ion-list>
            </ion-col>
          </ion-row>
        </ion-grid>
      </ion-content>
    ];
  }
}
