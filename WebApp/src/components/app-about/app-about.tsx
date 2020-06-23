import { Component, Element, Prop, State, h } from '@stencil/core';
import { AjaxService } from "../../services/ajax-service";
import { DefaultsService } from "../../services/defaults-service";

@Component({
  tag: 'app-about',
  styleUrl: 'app-about.scss',
})
export class AppAbout {

  @Element() element: HTMLElement;

  @State() settings: Settings = {};

  @Prop() isDisabled: boolean = true;

  async componentWillLoad() {
    this.settings = {...await DefaultsService.get()};

    this.isDisabled = true;
  }

  async componentDidLoad() {
    let settingsData = await AjaxService.getSettings();
    if (settingsData != null) {
      if (settingsData.success) {
        this.isDisabled = false;
        this.settings = {...settingsData.result};
      }
    }
  }

  dismiss() {
    (this.element.closest('ion-modal') as any).dismiss();
  }

  render() {
    return [
      <ion-header>
        <ion-toolbar>
          <ion-title>About</ion-title>
          <ion-buttons slot="end">
            <ion-button onClick={() => this.dismiss()}>Close</ion-button>
          </ion-buttons>
        </ion-toolbar>
      </ion-header>,

      <ion-content>
        <div class="about-header">
          <ion-img src="/assets/icon/icon-512x512.png" alt="infinity logo"/>
          <h1>Wordclock</h1>
          <h6><span style={{display: this.isDisabled ? 'none' : 'block'}}>Version {this.settings.version}</span></h6>
          <ion-icon name="arrow-down" color="light" class="scroll-down"/>
        </div>
        <div class="ion-padding about-info">
          <p><strong>Information according to § 5 TMG:</strong></p>
          <p>Carsten Walther<br/>Triftweg 1<br/>04277 Leipzig</p>
          <p><strong>Responsible for content<br/>according to § 55 Abs. 2 RStV:</strong></p>
          <p>Carsten Walther</p>
          <hr/>
          <p><strong>Haftungsausschluss (Disclaimer)</strong></p>
          <p><strong>Haftung für Inhalte</strong></p>
          <p>Als Diensteanbieter sind wir gemäß § 7 Abs.1 TMG für eigene Inhalte auf diesen Seiten nach den allgemeinen
            Gesetzen verantwortlich. Nach §§ 8 bis 10 TMG sind wir als Diensteanbieter jedoch nicht verpflichtet,
            übermittelte oder gespeicherte fremde Informationen zu überwachen oder nach Umständen zu forschen, die auf
            eine rechtswidrige Tätigkeit hinweisen. Verpflichtungen zur Entfernung oder Sperrung der Nutzung von
            Informationen nach den allgemeinen Gesetzen bleiben hiervon unberührt. Eine diesbezügliche Haftung ist
            jedoch erst ab dem Zeitpunkt der Kenntnis einer konkreten Rechtsverletzung möglich. Bei Bekanntwerden von
            entsprechenden Rechtsverletzungen werden wir diese Inhalte umgehend entfernen.</p>
          <p><strong>Haftung für Links</strong></p>
          <p>Unser Angebot enthält Links zu externen Webseiten Dritter, auf deren Inhalte wir keinen Einfluss haben.
            Deshalb können wir für diese fremden Inhalte auch keine Gewähr übernehmen. Für die Inhalte der verlinkten
            Seiten ist stets der jeweilige Anbieter oder Betreiber der Seiten verantwortlich. Die verlinkten Seiten
            wurden zum Zeitpunkt der Verlinkung auf mögliche Rechtsverstöße überprüft. Rechtswidrige Inhalte waren zum
            Zeitpunkt der Verlinkung nicht erkennbar. Eine permanente inhaltliche Kontrolle der verlinkten Seiten ist
            jedoch ohne konkrete Anhaltspunkte einer Rechtsverletzung nicht zumutbar. Bei Bekanntwerden von
            Rechtsverletzungen werden wir derartige Links umgehend entfernen.</p>
          <p><strong>Urheberrecht</strong></p>
          <p>Die durch die Seitenbetreiber erstellten Inhalte und Werke auf diesen Seiten unterliegen dem deutschen
            Urheberrecht. Die Vervielfältigung, Bearbeitung, Verbreitung und jede Art der Verwertung außerhalb der
            Grenzen des Urheberrechtes bedürfen der schriftlichen Zustimmung des jeweiligen Autors bzw. Erstellers.
            Downloads und Kopien dieser Seite sind nur für den privaten, nicht kommerziellen Gebrauch gestattet. Soweit
            die Inhalte auf dieser Seite nicht vom Betreiber erstellt wurden, werden die Urheberrechte Dritter beachtet.
            Insbesondere werden Inhalte Dritter als solche gekennzeichnet. Sollten Sie trotzdem auf eine
            Urheberrechtsverletzung aufmerksam werden, bitten wir um einen entsprechenden Hinweis. Bei Bekanntwerden von
            Rechtsverletzungen werden wir derartige Inhalte umgehend entfernen.</p>
        </div>
      </ion-content>
    ];
  }
}
