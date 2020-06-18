/**
 * AjaxServiceController
 */
export class AjaxServiceController {

  /**
   * baseUrl
   */
  public baseUrl: string = location.origin + '/';
  //public baseUrl: string = 'http://wordclock.local/';

  /**
   * getSettings
   */
  getSettings = async (): Promise<any> => {
    return fetch(this.baseUrl + 'settings.json')
      .then(response => {
        if (!response.ok) {
          throw new Error(response.statusText);
        }
        return response.json();
      })
      .then(data => {
        return data;
      });
  };

  /**
   * sendCommand
   *
   * @param field
   * @param value
   */
  setSettings = async (field: string, value: any): Promise<any> => {
    console.log(field, value);
  };
}

export const AjaxService = new AjaxServiceController();
