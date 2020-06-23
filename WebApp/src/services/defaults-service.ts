/**
 * DefaultsServiceController
 */
export class DefaultsServiceController {

  /**
   * file
   */
  private file: string = '/assets/data/defaults.json';

  /**
   * get
   */
  get = async (): Promise<Settings> => {
    return fetch(this.file)
      .then(response => response.json())
      .then(data => {
        return data;
      });
  };
}

export const DefaultsService = new DefaultsServiceController();
